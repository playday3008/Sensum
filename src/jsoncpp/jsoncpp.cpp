/// Json-cpp amalgated source (http://jsoncpp.sourceforge.net/).
/// It is intended to be used with #include "json/json.h"

// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////

/*
The JsonCpp library's source code, including accompanying documentation, 
tests and demonstration applications, are licensed under the following
conditions...

The author (Baptiste Lepilleur) explicitly disclaims copyright in all 
jurisdictions which recognize such a disclaimer. In such jurisdictions, 
this software is released into the Public Domain.

In jurisdictions which do not recognize Public Domain property (e.g. Germany as of
2010), this software is Copyright (c) 2007-2010 by Baptiste Lepilleur, and is
released under the terms of the MIT License (see below).

In jurisdictions which recognize Public Domain property, the user of this 
software may choose to accept it either as 1) Public Domain, 2) under the 
conditions of the MIT License (see below), or 3) under the terms of dual 
Public Domain/MIT License conditions described here, as they choose.

The MIT License is about as close to Public Domain as a license can get, and is
described in clear, concise terms at:

   http://en.wikipedia.org/wiki/MIT_License
   
The full text of the MIT License follows:

========================================================================
Copyright (c) 2007-2010 Baptiste Lepilleur

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
========================================================================
(END LICENSE TEXT)

The MIT license is compatible with both the GPL and commercial
software, affording one all of the rights of Public Domain with the
minor nuisance of being required to keep the above copyright notice
and license text in the source code. Note also that by accepting the
Public Domain "license" you can re-license your copy using whatever
license you like.

*/

// //////////////////////////////////////////////////////////////////////
// End of content of file: LICENSE
// //////////////////////////////////////////////////////////////////////






#include "json.h"

#ifndef JSON_IS_AMALGAMATION
#error "Compile with -I PATH_TO_JSON_DIRECTORY"
#endif


// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef LIB_JSONCPP_JSON_TOOL_H_INCLUDED
#define LIB_JSONCPP_JSON_TOOL_H_INCLUDED

#ifndef NO_LOCALE_SUPPORT
#include <clocale>
#endif

/* This header provides common string manipulation support, such as UTF-8,
 * portable conversion from/to string...
 *
 * It is an internal header that must not be exposed.
 */

namespace Json {
static char getDecimalPoint() {
#ifdef NO_LOCALE_SUPPORT
  return '\0';
#else
  struct lconv* lc = localeconv();
  return lc ? *(lc->decimal_point) : '\0';
#endif
}

/// Converts a unicode code-point to UTF-8.
static inline JSONCPP_STRING codePointToUTF8(unsigned int cp) {
  JSONCPP_STRING result;

  // based on description from http://en.wikipedia.org/wiki/UTF-8

  if (cp <= 0x7f) {
    result.resize(1);
    result[0] = static_cast<char>(cp);
  } else if (cp <= 0x7FF) {
    result.resize(2);
    result[1] = static_cast<char>(0x80 | (0x3f & cp));
    result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
  } else if (cp <= 0xFFFF) {
    result.resize(3);
    result[2] = static_cast<char>(0x80 | (0x3f & cp));
    result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
    result[0] = static_cast<char>(0xE0 | (0xf & (cp >> 12)));
  } else if (cp <= 0x10FFFF) {
    result.resize(4);
    result[3] = static_cast<char>(0x80 | (0x3f & cp));
    result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
    result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
    result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
  }

  return result;
}

/// Returns true if ch is a control character (in range [1,31]).
static inline bool isControlCharacter(char ch) { return ch > 0 && ch <= 0x1F; }

enum {
  /// Constant that specify the size of the buffer that must be passed to
  /// uintToString.
  uintToStringBufferSize = 3 * sizeof(LargestUInt) + 1
};

// Defines a char buffer for use with uintToString().
typedef char UIntToStringBuffer[uintToStringBufferSize];

/** Converts an unsigned integer to string.
 * @param value Unsigned interger to convert to string
 * @param current Input/Output string buffer.
 *        Must have at least uintToStringBufferSize chars free.
 */
static inline void uintToString(LargestUInt value, char*& current) {
  *--current = 0;
  do {
    *--current = static_cast<char>(value % 10U + static_cast<unsigned>('0'));
    value /= 10;
  } while (value != 0);
}

/** Change ',' to '.' everywhere in buffer.
 *
 * We had a sophisticated way, but it did not work in WinCE.
 * @see https://github.com/open-source-parsers/jsoncpp/pull/9
 */
static inline void fixNumericLocale(char* begin, char* end) {
  while (begin < end) {
    if (*begin == ',') {
      *begin = '.';
    }
    ++begin;
  }
}

static inline void fixNumericLocaleInput(char* begin, char* end) {
  char decimalPoint = getDecimalPoint();
  if (decimalPoint != '\0' && decimalPoint != '.') {
    while (begin < end) {
      if (*begin == '.') {
        *begin = decimalPoint;
      }
      ++begin;
    }
  }
}

} // namespace Json {

#endif // LIB_JSONCPP_JSON_TOOL_H_INCLUDED

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_tool.h
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2011 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/reader.h>
#include <json/value.h>
#include "json_tool.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <utility>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <istream>
#include <sstream>
#include <memory>
#include <set>
#include <limits>

#if defined(_MSC_VER)
#if !defined(WINCE) && defined(__STDC_SECURE_LIB__) && _MSC_VER >= 1500 // VC++ 9.0 and above 
#define snprintf sprintf_s
#elif _MSC_VER >= 1900 // VC++ 14.0 and above
#define snprintf std::snprintf
#else
#define snprintf _snprintf
#endif
#elif defined(__ANDROID__) || defined(__QNXNTO__)
#define snprintf snprintf
#elif __cplusplus >= 201103L
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
#define snprintf std::snprintf
#endif
#endif

#if defined(__QNXNTO__)
#define sscanf std::sscanf
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC++ 8.0
// Disable warning about strdup being deprecated.
#pragma warning(disable : 4996)
#endif

static int const stackLimit_g = 1000;
static int       stackDepth_g = 0;  // see readValue()

namespace Json {

#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
typedef std::unique_ptr<CharReader> CharReaderPtr;
#else
typedef std::auto_ptr<CharReader>   CharReaderPtr;
#endif

// Implementation of class Features
// ////////////////////////////////

Features::Features()
    : allowComments_(true), strictRoot_(false),
      allowDroppedNullPlaceholders_(false), allowNumericKeys_(false) {}

Features Features::all() { return Features(); }

Features Features::strictMode() {
  Features features;
  features.allowComments_ = false;
  features.strictRoot_ = true;
  features.allowDroppedNullPlaceholders_ = false;
  features.allowNumericKeys_ = false;
  return features;
}

// Implementation of class Reader
// ////////////////////////////////

static bool containsNewLine(Reader::Location begin, Reader::Location end) {
  for (; begin < end; ++begin)
    if (*begin == '\n' || *begin == '\r')
      return true;
  return false;
}

// Class Reader
// //////////////////////////////////////////////////////////////////

Reader::Reader()
    : errors_(), document_(), begin_(), end_(), current_(), lastValueEnd_(),
      lastValue_(), commentsBefore_(), features_(Features::all()),
      collectComments_() {}

Reader::Reader(const Features& features)
    : errors_(), document_(), begin_(), end_(), current_(), lastValueEnd_(),
      lastValue_(), commentsBefore_(), features_(features), collectComments_() {
}

bool
Reader::parse(const std::string& document, Value& root, bool collectComments) {
  JSONCPP_STRING documentCopy(document.data(), document.data() + document.capacity());
  std::swap(documentCopy, document_);
  const char* begin = document_.c_str();
  const char* end = begin + document_.length();
  return parse(begin, end, root, collectComments);
}

bool Reader::parse(std::istream& sin, Value& root, bool collectComments) {
  // std::istream_iterator<char> begin(sin);
  // std::istream_iterator<char> end;
  // Those would allow streamed input from a file, if parse() were a
  // template function.

  // Since JSONCPP_STRING is reference-counted, this at least does not
  // create an extra copy.
  JSONCPP_STRING doc;
  std::getline(sin, doc, (char)EOF);
  return parse(doc.data(), doc.data() + doc.size(), root, collectComments);
}

bool Reader::parse(const char* beginDoc,
                   const char* endDoc,
                   Value& root,
                   bool collectComments) {
  if (!features_.allowComments_) {
    collectComments = false;
  }

  begin_ = beginDoc;
  end_ = endDoc;
  collectComments_ = collectComments;
  current_ = begin_;
  lastValueEnd_ = 0;
  lastValue_ = 0;
  commentsBefore_ = "";
  errors_.clear();
  while (!nodes_.empty())
    nodes_.pop();
  nodes_.push(&root);

  stackDepth_g = 0;  // Yes, this is bad coding, but options are limited.
  bool successful = readValue();
  Token token;
  skipCommentTokens(token);
  if (collectComments_ && !commentsBefore_.empty())
    root.setComment(commentsBefore_, commentAfter);
  if (features_.strictRoot_) {
    if (!root.isArray() && !root.isObject()) {
      // Set error location to start of doc, ideally should be first token found
      // in doc
      token.type_ = tokenError;
      token.start_ = beginDoc;
      token.end_ = endDoc;
      addError(
          "A valid JSON document must be either an array or an object value.",
          token);
      return false;
    }
  }
  return successful;
}

bool Reader::readValue() {
  // This is a non-reentrant way to support a stackLimit. Terrible!
  // But this deprecated class has a security problem: Bad input can
  // cause a seg-fault. This seems like a fair, binary-compatible way
  // to prevent the problem.
  if (stackDepth_g >= stackLimit_g) throwRuntimeError("Exceeded stackLimit in readValue().");
  ++stackDepth_g;

  Token token;
  skipCommentTokens(token);
  bool successful = true;

  if (collectComments_ && !commentsBefore_.empty()) {
    currentValue().setComment(commentsBefore_, commentBefore);
    commentsBefore_ = "";
  }

  switch (token.type_) {
  case tokenObjectBegin:
    successful = readObject(token);
    currentValue().setOffsetLimit(current_ - begin_);
    break;
  case tokenArrayBegin:
    successful = readArray(token);
    currentValue().setOffsetLimit(current_ - begin_);
    break;
  case tokenNumber:
    successful = decodeNumber(token);
    break;
  case tokenString:
    successful = decodeString(token);
    break;
  case tokenTrue:
    {
    Value v(true);
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenFalse:
    {
    Value v(false);
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenNull:
    {
    Value v;
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenArraySeparator:
  case tokenObjectEnd:
  case tokenArrayEnd:
    if (features_.allowDroppedNullPlaceholders_) {
      // "Un-read" the current token and mark the current value as a null
      // token.
      current_--;
      Value v;
      currentValue().swapPayload(v);
      currentValue().setOffsetStart(current_ - begin_ - 1);
      currentValue().setOffsetLimit(current_ - begin_);
      break;
    } // Else, fall through...
  default:
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    return addError("Syntax error: value, object or array expected.", token);
  }

  if (collectComments_) {
    lastValueEnd_ = current_;
    lastValue_ = &currentValue();
  }

  --stackDepth_g;
  return successful;
}

void Reader::skipCommentTokens(Token& token) {
  if (features_.allowComments_) {
    do {
      readToken(token);
    } while (token.type_ == tokenComment);
  } else {
    readToken(token);
  }
}

bool Reader::readToken(Token& token) {
  skipSpaces();
  token.start_ = current_;
  Char c = getNextChar();
  bool ok = true;
  switch (c) {
  case '{':
    token.type_ = tokenObjectBegin;
    break;
  case '}':
    token.type_ = tokenObjectEnd;
    break;
  case '[':
    token.type_ = tokenArrayBegin;
    break;
  case ']':
    token.type_ = tokenArrayEnd;
    break;
  case '"':
    token.type_ = tokenString;
    ok = readString();
    break;
  case '/':
    token.type_ = tokenComment;
    ok = readComment();
    break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-':
    token.type_ = tokenNumber;
    readNumber();
    break;
  case 't':
    token.type_ = tokenTrue;
    ok = match("rue", 3);
    break;
  case 'f':
    token.type_ = tokenFalse;
    ok = match("alse", 4);
    break;
  case 'n':
    token.type_ = tokenNull;
    ok = match("ull", 3);
    break;
  case ',':
    token.type_ = tokenArraySeparator;
    break;
  case ':':
    token.type_ = tokenMemberSeparator;
    break;
  case 0:
    token.type_ = tokenEndOfStream;
    break;
  default:
    ok = false;
    break;
  }
  if (!ok)
    token.type_ = tokenError;
  token.end_ = current_;
  return true;
}

void Reader::skipSpaces() {
  while (current_ != end_) {
    Char c = *current_;
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
      ++current_;
    else
      break;
  }
}

bool Reader::match(Location pattern, int patternLength) {
  if (end_ - current_ < patternLength)
    return false;
  int index = patternLength;
  while (index--)
    if (current_[index] != pattern[index])
      return false;
  current_ += patternLength;
  return true;
}

bool Reader::readComment() {
  Location commentBegin = current_ - 1;
  Char c = getNextChar();
  bool successful = false;
  if (c == '*')
    successful = readCStyleComment();
  else if (c == '/')
    successful = readCppStyleComment();
  if (!successful)
    return false;

  if (collectComments_) {
    CommentPlacement placement = commentBefore;
    if (lastValueEnd_ && !containsNewLine(lastValueEnd_, commentBegin)) {
      if (c != '*' || !containsNewLine(commentBegin, current_))
        placement = commentAfterOnSameLine;
    }

    addComment(commentBegin, current_, placement);
  }
  return true;
}

static JSONCPP_STRING normalizeEOL(Reader::Location begin, Reader::Location end) {
  JSONCPP_STRING normalized;
  normalized.reserve(static_cast<size_t>(end - begin));
  Reader::Location current = begin;
  while (current != end) {
    char c = *current++;
    if (c == '\r') {
      if (current != end && *current == '\n')
         // convert dos EOL
         ++current;
      // convert Mac EOL
      normalized += '\n';
    } else {
      normalized += c;
    }
  }
  return normalized;
}

void
Reader::addComment(Location begin, Location end, CommentPlacement placement) {
  assert(collectComments_);
  const JSONCPP_STRING& normalized = normalizeEOL(begin, end);
  if (placement == commentAfterOnSameLine) {
    assert(lastValue_ != 0);
    lastValue_->setComment(normalized, placement);
  } else {
    commentsBefore_ += normalized;
  }
}

bool Reader::readCStyleComment() {
  while ((current_ + 1) < end_) {
    Char c = getNextChar();
    if (c == '*' && *current_ == '/')
      break;
  }
  return getNextChar() == '/';
}

bool Reader::readCppStyleComment() {
  while (current_ != end_) {
    Char c = getNextChar();
    if (c == '\n')
      break;
    if (c == '\r') {
      // Consume DOS EOL. It will be normalized in addComment.
      if (current_ != end_ && *current_ == '\n')
        getNextChar();
      // Break on Moc OS 9 EOL.
      break;
    }
  }
  return true;
}

void Reader::readNumber() {
  const char *p = current_;
  char c = '0'; // stopgap for already consumed character
  // integral part
  while (c >= '0' && c <= '9')
    c = (current_ = p) < end_ ? *p++ : '\0';
  // fractional part
  if (c == '.') {
    c = (current_ = p) < end_ ? *p++ : '\0';
    while (c >= '0' && c <= '9')
      c = (current_ = p) < end_ ? *p++ : '\0';
  }
  // exponential part
  if (c == 'e' || c == 'E') {
    c = (current_ = p) < end_ ? *p++ : '\0';
    if (c == '+' || c == '-')
      c = (current_ = p) < end_ ? *p++ : '\0';
    while (c >= '0' && c <= '9')
      c = (current_ = p) < end_ ? *p++ : '\0';
  }
}

bool Reader::readString() {
  Char c = '\0';
  while (current_ != end_) {
    c = getNextChar();
    if (c == '\\')
      getNextChar();
    else if (c == '"')
      break;
  }
  return c == '"';
}

bool Reader::readObject(Token& tokenStart) {
  Token tokenName;
  JSONCPP_STRING name;
  Value init(objectValue);
  currentValue().swapPayload(init);
  currentValue().setOffsetStart(tokenStart.start_ - begin_);
  while (readToken(tokenName)) {
    bool initialTokenOk = true;
    while (tokenName.type_ == tokenComment && initialTokenOk)
      initialTokenOk = readToken(tokenName);
    if (!initialTokenOk)
      break;
    if (tokenName.type_ == tokenObjectEnd && name.empty()) // empty object
      return true;
    name = "";
    if (tokenName.type_ == tokenString) {
      if (!decodeString(tokenName, name))
        return recoverFromError(tokenObjectEnd);
    } else if (tokenName.type_ == tokenNumber && features_.allowNumericKeys_) {
      Value numberName;
      if (!decodeNumber(tokenName, numberName))
        return recoverFromError(tokenObjectEnd);
      name = JSONCPP_STRING(numberName.asCString());
    } else {
      break;
    }

    Token colon;
    if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
      return addErrorAndRecover(
          "Missing ':' after object member name", colon, tokenObjectEnd);
    }
    Value& value = currentValue()[name];
    nodes_.push(&value);
    bool ok = readValue();
    nodes_.pop();
    if (!ok) // error already set
      return recoverFromError(tokenObjectEnd);

    Token comma;
    if (!readToken(comma) ||
        (comma.type_ != tokenObjectEnd && comma.type_ != tokenArraySeparator &&
         comma.type_ != tokenComment)) {
      return addErrorAndRecover(
          "Missing ',' or '}' in object declaration", comma, tokenObjectEnd);
    }
    bool finalizeTokenOk = true;
    while (comma.type_ == tokenComment && finalizeTokenOk)
      finalizeTokenOk = readToken(comma);
    if (comma.type_ == tokenObjectEnd)
      return true;
  }
  return addErrorAndRecover(
      "Missing '}' or object member name", tokenName, tokenObjectEnd);
}

bool Reader::readArray(Token& tokenStart) {
  Value init(arrayValue);
  currentValue().swapPayload(init);
  currentValue().setOffsetStart(tokenStart.start_ - begin_);
  skipSpaces();
  if (current_ != end_ && *current_ == ']') // empty array
  {
    Token endArray;
    readToken(endArray);
    return true;
  }
  int index = 0;
  for (;;) {
    Value& value = currentValue()[index++];
    nodes_.push(&value);
    bool ok = readValue();
    nodes_.pop();
    if (!ok) // error already set
      return recoverFromError(tokenArrayEnd);

    Token token;
    // Accept Comment after last item in the array.
    ok = readToken(token);
    while (token.type_ == tokenComment && ok) {
      ok = readToken(token);
    }
    bool badTokenType =
        (token.type_ != tokenArraySeparator && token.type_ != tokenArrayEnd);
    if (!ok || badTokenType) {
      return addErrorAndRecover(
          "Missing ',' or ']' in array declaration", token, tokenArrayEnd);
    }
    if (token.type_ == tokenArrayEnd)
      break;
  }
  return true;
}

bool Reader::decodeNumber(Token& token) {
  Value decoded;
  if (!decodeNumber(token, decoded))
    return false;
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool Reader::decodeNumber(Token& token, Value& decoded) {
  // Attempts to parse the number as an integer. If the number is
  // larger than the maximum supported value of an integer then
  // we decode the number as a double.
  Location current = token.start_;
  bool isNegative = *current == '-';
  if (isNegative)
    ++current;
  // TODO: Help the compiler do the div and mod at compile time or get rid of them.
  Value::LargestUInt maxIntegerValue =
      isNegative ? Value::LargestUInt(Value::maxLargestInt) + 1
                 : Value::maxLargestUInt;
  Value::LargestUInt threshold = maxIntegerValue / 10;
  Value::LargestUInt value = 0;
  while (current < token.end_) {
    Char c = *current++;
    if (c < '0' || c > '9')
      return decodeDouble(token, decoded);
    Value::UInt digit(static_cast<Value::UInt>(c - '0'));
    if (value >= threshold) {
      // We've hit or exceeded the max value divided by 10 (rounded down). If
      // a) we've only just touched the limit, b) this is the last digit, and
      // c) it's small enough to fit in that rounding delta, we're okay.
      // Otherwise treat this number as a double to avoid overflow.
      if (value > threshold || current != token.end_ ||
          digit > maxIntegerValue % 10) {
        return decodeDouble(token, decoded);
      }
    }
    value = value * 10 + digit;
  }
  if (isNegative && value == maxIntegerValue)
    decoded = Value::minLargestInt;
  else if (isNegative)
    decoded = -Value::LargestInt(value);
  else if (value <= Value::LargestUInt(Value::maxInt))
    decoded = Value::LargestInt(value);
  else
    decoded = value;
  return true;
}

bool Reader::decodeDouble(Token& token) {
  Value decoded;
  if (!decodeDouble(token, decoded))
    return false;
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool Reader::decodeDouble(Token& token, Value& decoded) {
  double value = 0;
  JSONCPP_STRING buffer(token.start_, token.end_);
  JSONCPP_ISTRINGSTREAM is(buffer);
  if (!(is >> value))
    return addError("'" + JSONCPP_STRING(token.start_, token.end_) +
                        "' is not a number.",
                    token);
  decoded = value;
  return true;
}

bool Reader::decodeString(Token& token) {
  JSONCPP_STRING decoded_string;
  if (!decodeString(token, decoded_string))
    return false;
  Value decoded(decoded_string);
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool Reader::decodeString(Token& token, JSONCPP_STRING& decoded) {
  decoded.reserve(static_cast<size_t>(token.end_ - token.start_ - 2));
  Location current = token.start_ + 1; // skip '"'
  Location end = token.end_ - 1;       // do not include '"'
  while (current != end) {
    Char c = *current++;
    if (c == '"')
      break;
    else if (c == '\\') {
      if (current == end)
        return addError("Empty escape sequence in string", token, current);
      Char escape = *current++;
      switch (escape) {
      case '"':
        decoded += '"';
        break;
      case '/':
        decoded += '/';
        break;
      case '\\':
        decoded += '\\';
        break;
      case 'b':
        decoded += '\b';
        break;
      case 'f':
        decoded += '\f';
        break;
      case 'n':
        decoded += '\n';
        break;
      case 'r':
        decoded += '\r';
        break;
      case 't':
        decoded += '\t';
        break;
      case 'u': {
        unsigned int unicode;
        if (!decodeUnicodeCodePoint(token, current, end, unicode))
          return false;
        decoded += codePointToUTF8(unicode);
      } break;
      default:
        return addError("Bad escape sequence in string", token, current);
      }
    } else {
      decoded += c;
    }
  }
  return true;
}

bool Reader::decodeUnicodeCodePoint(Token& token,
                                    Location& current,
                                    Location end,
                                    unsigned int& unicode) {

  if (!decodeUnicodeEscapeSequence(token, current, end, unicode))
    return false;
  if (unicode >= 0xD800 && unicode <= 0xDBFF) {
    // surrogate pairs
    if (end - current < 6)
      return addError(
          "additional six characters expected to parse unicode surrogate pair.",
          token,
          current);
    unsigned int surrogatePair;
    if (*(current++) == '\\' && *(current++) == 'u') {
      if (decodeUnicodeEscapeSequence(token, current, end, surrogatePair)) {
        unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
      } else
        return false;
    } else
      return addError("expecting another \\u token to begin the second half of "
                      "a unicode surrogate pair",
                      token,
                      current);
  }
  return true;
}

bool Reader::decodeUnicodeEscapeSequence(Token& token,
                                         Location& current,
                                         Location end,
                                         unsigned int& ret_unicode) {
  if (end - current < 4)
    return addError(
        "Bad unicode escape sequence in string: four digits expected.",
        token,
        current);
  int unicode = 0;
  for (int index = 0; index < 4; ++index) {
    Char c = *current++;
    unicode *= 16;
    if (c >= '0' && c <= '9')
      unicode += c - '0';
    else if (c >= 'a' && c <= 'f')
      unicode += c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      unicode += c - 'A' + 10;
    else
      return addError(
          "Bad unicode escape sequence in string: hexadecimal digit expected.",
          token,
          current);
  }
  ret_unicode = static_cast<unsigned int>(unicode);
  return true;
}

bool
Reader::addError(const JSONCPP_STRING& message, Token& token, Location extra) {
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = extra;
  errors_.push_back(info);
  return false;
}

bool Reader::recoverFromError(TokenType skipUntilToken) {
  size_t const errorCount = errors_.size();
  Token skip;
  for (;;) {
    if (!readToken(skip))
      errors_.resize(errorCount); // discard errors caused by recovery
    if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream)
      break;
  }
  errors_.resize(errorCount);
  return false;
}

bool Reader::addErrorAndRecover(const JSONCPP_STRING& message,
                                Token& token,
                                TokenType skipUntilToken) {
  addError(message, token);
  return recoverFromError(skipUntilToken);
}

Value& Reader::currentValue() { return *(nodes_.top()); }

Reader::Char Reader::getNextChar() {
  if (current_ == end_)
    return 0;
  return *current_++;
}

void Reader::getLocationLineAndColumn(Location location,
                                      int& line,
                                      int& column) const {
  Location current = begin_;
  Location lastLineStart = current;
  line = 0;
  while (current < location && current != end_) {
    Char c = *current++;
    if (c == '\r') {
      if (*current == '\n')
        ++current;
      lastLineStart = current;
      ++line;
    } else if (c == '\n') {
      lastLineStart = current;
      ++line;
    }
  }
  // column & line start at 1
  column = int(location - lastLineStart) + 1;
  ++line;
}

JSONCPP_STRING Reader::getLocationLineAndColumn(Location location) const {
  int line, column;
  getLocationLineAndColumn(location, line, column);
  char buffer[18 + 16 + 16 + 1];
  snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
  return buffer;
}

// Deprecated. Preserved for backward compatibility
JSONCPP_STRING Reader::getFormatedErrorMessages() const {
  return getFormattedErrorMessages();
}

JSONCPP_STRING Reader::getFormattedErrorMessages() const {
  JSONCPP_STRING formattedMessage;
  for (Errors::const_iterator itError = errors_.begin();
       itError != errors_.end();
       ++itError) {
    const ErrorInfo& error = *itError;
    formattedMessage +=
        "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
    formattedMessage += "  " + error.message_ + "\n";
    if (error.extra_)
      formattedMessage +=
          "See " + getLocationLineAndColumn(error.extra_) + " for detail.\n";
  }
  return formattedMessage;
}

std::vector<Reader::StructuredError> Reader::getStructuredErrors() const {
  std::vector<Reader::StructuredError> allErrors;
  for (Errors::const_iterator itError = errors_.begin();
       itError != errors_.end();
       ++itError) {
    const ErrorInfo& error = *itError;
    Reader::StructuredError structured;
    structured.offset_start = error.token_.start_ - begin_;
    structured.offset_limit = error.token_.end_ - begin_;
    structured.message = error.message_;
    allErrors.push_back(structured);
  }
  return allErrors;
}

bool Reader::pushError(const Value& value, const JSONCPP_STRING& message) {
  ptrdiff_t const length = end_ - begin_;
  if(value.getOffsetStart() > length
    || value.getOffsetLimit() > length)
    return false;
  Token token;
  token.type_ = tokenError;
  token.start_ = begin_ + value.getOffsetStart();
  token.end_ = end_ + value.getOffsetLimit();
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = 0;
  errors_.push_back(info);
  return true;
}

bool Reader::pushError(const Value& value, const JSONCPP_STRING& message, const Value& extra) {
  ptrdiff_t const length = end_ - begin_;
  if(value.getOffsetStart() > length
    || value.getOffsetLimit() > length
    || extra.getOffsetLimit() > length)
    return false;
  Token token;
  token.type_ = tokenError;
  token.start_ = begin_ + value.getOffsetStart();
  token.end_ = begin_ + value.getOffsetLimit();
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = begin_ + extra.getOffsetStart();
  errors_.push_back(info);
  return true;
}

bool Reader::good() const {
  return !errors_.size();
}

// exact copy of Features
class OurFeatures {
public:
  static OurFeatures all();
  bool allowComments_;
  bool strictRoot_;
  bool allowDroppedNullPlaceholders_;
  bool allowNumericKeys_;
  bool allowSingleQuotes_;
  bool failIfExtra_;
  bool rejectDupKeys_;
  bool allowSpecialFloats_;
  int stackLimit_;
};  // OurFeatures

// exact copy of Implementation of class Features
// ////////////////////////////////

OurFeatures OurFeatures::all() { return OurFeatures(); }

// Implementation of class Reader
// ////////////////////////////////

// exact copy of Reader, renamed to OurReader
class OurReader {
public:
  typedef char Char;
  typedef const Char* Location;
  struct StructuredError {
    ptrdiff_t offset_start;
    ptrdiff_t offset_limit;
    JSONCPP_STRING message;
  };

  OurReader(OurFeatures const& features);
  bool parse(const char* beginDoc,
             const char* endDoc,
             Value& root,
             bool collectComments = true);
  JSONCPP_STRING getFormattedErrorMessages() const;
  std::vector<StructuredError> getStructuredErrors() const;
  bool pushError(const Value& value, const JSONCPP_STRING& message);
  bool pushError(const Value& value, const JSONCPP_STRING& message, const Value& extra);
  bool good() const;

private:
  OurReader(OurReader const&);  // no impl
  void operator=(OurReader const&);  // no impl

  enum TokenType {
    tokenEndOfStream = 0,
    tokenObjectBegin,
    tokenObjectEnd,
    tokenArrayBegin,
    tokenArrayEnd,
    tokenString,
    tokenNumber,
    tokenTrue,
    tokenFalse,
    tokenNull,
    tokenNaN,
    tokenPosInf,
    tokenNegInf,
    tokenArraySeparator,
    tokenMemberSeparator,
    tokenComment,
    tokenError
  };

  class Token {
  public:
    TokenType type_;
    Location start_;
    Location end_;
  };

  class ErrorInfo {
  public:
    Token token_;
    JSONCPP_STRING message_;
    Location extra_;
  };

  typedef std::deque<ErrorInfo> Errors;

  bool readToken(Token& token);
  void skipSpaces();
  bool match(Location pattern, int patternLength);
  bool readComment();
  bool readCStyleComment();
  bool readCppStyleComment();
  bool readString();
  bool readStringSingleQuote();
  bool readNumber(bool checkInf);
  bool readValue();
  bool readObject(Token& token);
  bool readArray(Token& token);
  bool decodeNumber(Token& token);
  bool decodeNumber(Token& token, Value& decoded);
  bool decodeString(Token& token);
  bool decodeString(Token& token, JSONCPP_STRING& decoded);
  bool decodeDouble(Token& token);
  bool decodeDouble(Token& token, Value& decoded);
  bool decodeUnicodeCodePoint(Token& token,
                              Location& current,
                              Location end,
                              unsigned int& unicode);
  bool decodeUnicodeEscapeSequence(Token& token,
                                   Location& current,
                                   Location end,
                                   unsigned int& unicode);
  bool addError(const JSONCPP_STRING& message, Token& token, Location extra = 0);
  bool recoverFromError(TokenType skipUntilToken);
  bool addErrorAndRecover(const JSONCPP_STRING& message,
                          Token& token,
                          TokenType skipUntilToken);
  void skipUntilSpace();
  Value& currentValue();
  Char getNextChar();
  void
  getLocationLineAndColumn(Location location, int& line, int& column) const;
  JSONCPP_STRING getLocationLineAndColumn(Location location) const;
  void addComment(Location begin, Location end, CommentPlacement placement);
  void skipCommentTokens(Token& token);

  typedef std::stack<Value*> Nodes;
  Nodes nodes_;
  Errors errors_;
  JSONCPP_STRING document_;
  Location begin_;
  Location end_;
  Location current_;
  Location lastValueEnd_;
  Value* lastValue_;
  JSONCPP_STRING commentsBefore_;
  int stackDepth_;

  OurFeatures const features_;
  bool collectComments_;
};  // OurReader

// complete copy of Read impl, for OurReader

OurReader::OurReader(OurFeatures const& features)
    : errors_(), document_(), begin_(), end_(), current_(), lastValueEnd_(),
      lastValue_(), commentsBefore_(),
      stackDepth_(0),
      features_(features), collectComments_() {
}

bool OurReader::parse(const char* beginDoc,
                   const char* endDoc,
                   Value& root,
                   bool collectComments) {
  if (!features_.allowComments_) {
    collectComments = false;
  }

  begin_ = beginDoc;
  end_ = endDoc;
  collectComments_ = collectComments;
  current_ = begin_;
  lastValueEnd_ = 0;
  lastValue_ = 0;
  commentsBefore_ = "";
  errors_.clear();
  while (!nodes_.empty())
    nodes_.pop();
  nodes_.push(&root);

  stackDepth_ = 0;
  bool successful = readValue();
  Token token;
  skipCommentTokens(token);
  if (features_.failIfExtra_) {
    if ((features_.strictRoot_ || token.type_ != tokenError) && token.type_ != tokenEndOfStream) {
      addError("Extra non-whitespace after JSON value.", token);
      return false;
    }
  }
  if (collectComments_ && !commentsBefore_.empty())
    root.setComment(commentsBefore_, commentAfter);
  if (features_.strictRoot_) {
    if (!root.isArray() && !root.isObject()) {
      // Set error location to start of doc, ideally should be first token found
      // in doc
      token.type_ = tokenError;
      token.start_ = beginDoc;
      token.end_ = endDoc;
      addError(
          "A valid JSON document must be either an array or an object value.",
          token);
      return false;
    }
  }
  return successful;
}

bool OurReader::readValue() {
  if (stackDepth_ >= features_.stackLimit_) throwRuntimeError("Exceeded stackLimit in readValue().");
  ++stackDepth_;
  Token token;
  skipCommentTokens(token);
  bool successful = true;

  if (collectComments_ && !commentsBefore_.empty()) {
    currentValue().setComment(commentsBefore_, commentBefore);
    commentsBefore_ = "";
  }

  switch (token.type_) {
  case tokenObjectBegin:
    successful = readObject(token);
    currentValue().setOffsetLimit(current_ - begin_);
    break;
  case tokenArrayBegin:
    successful = readArray(token);
    currentValue().setOffsetLimit(current_ - begin_);
    break;
  case tokenNumber:
    successful = decodeNumber(token);
    break;
  case tokenString:
    successful = decodeString(token);
    break;
  case tokenTrue:
    {
    Value v(true);
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenFalse:
    {
    Value v(false);
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenNull:
    {
    Value v;
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenNaN:
    {
    Value v(std::numeric_limits<double>::quiet_NaN());
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenPosInf:
    {
    Value v(std::numeric_limits<double>::infinity());
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenNegInf:
    {
    Value v(-std::numeric_limits<double>::infinity());
    currentValue().swapPayload(v);
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    }
    break;
  case tokenArraySeparator:
  case tokenObjectEnd:
  case tokenArrayEnd:
    if (features_.allowDroppedNullPlaceholders_) {
      // "Un-read" the current token and mark the current value as a null
      // token.
      current_--;
      Value v;
      currentValue().swapPayload(v);
      currentValue().setOffsetStart(current_ - begin_ - 1);
      currentValue().setOffsetLimit(current_ - begin_);
      break;
    } // else, fall through ...
  default:
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    return addError("Syntax error: value, object or array expected.", token);
  }

  if (collectComments_) {
    lastValueEnd_ = current_;
    lastValue_ = &currentValue();
  }

  --stackDepth_;
  return successful;
}

void OurReader::skipCommentTokens(Token& token) {
  if (features_.allowComments_) {
    do {
      readToken(token);
    } while (token.type_ == tokenComment);
  } else {
    readToken(token);
  }
}

bool OurReader::readToken(Token& token) {
  skipSpaces();
  token.start_ = current_;
  Char c = getNextChar();
  bool ok = true;
  switch (c) {
  case '{':
    token.type_ = tokenObjectBegin;
    break;
  case '}':
    token.type_ = tokenObjectEnd;
    break;
  case '[':
    token.type_ = tokenArrayBegin;
    break;
  case ']':
    token.type_ = tokenArrayEnd;
    break;
  case '"':
    token.type_ = tokenString;
    ok = readString();
    break;
  case '\'':
    if (features_.allowSingleQuotes_) {
    token.type_ = tokenString;
    ok = readStringSingleQuote();
    break;
    } // else continue
  case '/':
    token.type_ = tokenComment;
    ok = readComment();
    break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    token.type_ = tokenNumber;
    readNumber(false);
    break;
  case '-':
    if (readNumber(true)) {
      token.type_ = tokenNumber;
    } else {
      token.type_ = tokenNegInf;
      ok = features_.allowSpecialFloats_ && match("nfinity", 7);
    }
    break;
  case 't':
    token.type_ = tokenTrue;
    ok = match("rue", 3);
    break;
  case 'f':
    token.type_ = tokenFalse;
    ok = match("alse", 4);
    break;
  case 'n':
    token.type_ = tokenNull;
    ok = match("ull", 3);
    break;
  case 'N':
    if (features_.allowSpecialFloats_) {
      token.type_ = tokenNaN;
      ok = match("aN", 2);
    } else {
      ok = false;
    }
    break;
  case 'I':
    if (features_.allowSpecialFloats_) {
      token.type_ = tokenPosInf;
      ok = match("nfinity", 7);
    } else {
      ok = false;
    }
    break;
  case ',':
    token.type_ = tokenArraySeparator;
    break;
  case ':':
    token.type_ = tokenMemberSeparator;
    break;
  case 0:
    token.type_ = tokenEndOfStream;
    break;
  default:
    ok = false;
    break;
  }
  if (!ok)
    token.type_ = tokenError;
  token.end_ = current_;
  return true;
}

void OurReader::skipSpaces() {
  while (current_ != end_) {
    Char c = *current_;
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
      ++current_;
    else
      break;
  }
}

bool OurReader::match(Location pattern, int patternLength) {
  if (end_ - current_ < patternLength)
    return false;
  int index = patternLength;
  while (index--)
    if (current_[index] != pattern[index])
      return false;
  current_ += patternLength;
  return true;
}

bool OurReader::readComment() {
  Location commentBegin = current_ - 1;
  Char c = getNextChar();
  bool successful = false;
  if (c == '*')
    successful = readCStyleComment();
  else if (c == '/')
    successful = readCppStyleComment();
  if (!successful)
    return false;

  if (collectComments_) {
    CommentPlacement placement = commentBefore;
    if (lastValueEnd_ && !containsNewLine(lastValueEnd_, commentBegin)) {
      if (c != '*' || !containsNewLine(commentBegin, current_))
        placement = commentAfterOnSameLine;
    }

    addComment(commentBegin, current_, placement);
  }
  return true;
}

void
OurReader::addComment(Location begin, Location end, CommentPlacement placement) {
  assert(collectComments_);
  const JSONCPP_STRING& normalized = normalizeEOL(begin, end);
  if (placement == commentAfterOnSameLine) {
    assert(lastValue_ != 0);
    lastValue_->setComment(normalized, placement);
  } else {
    commentsBefore_ += normalized;
  }
}

bool OurReader::readCStyleComment() {
  while ((current_ + 1) < end_) {
    Char c = getNextChar();
    if (c == '*' && *current_ == '/')
      break;
  }
  return getNextChar() == '/';
}

bool OurReader::readCppStyleComment() {
  while (current_ != end_) {
    Char c = getNextChar();
    if (c == '\n')
      break;
    if (c == '\r') {
      // Consume DOS EOL. It will be normalized in addComment.
      if (current_ != end_ && *current_ == '\n')
        getNextChar();
      // Break on Moc OS 9 EOL.
      break;
    }
  }
  return true;
}

bool OurReader::readNumber(bool checkInf) {
  const char *p = current_;
  if (checkInf && p != end_ && *p == 'I') {
    current_ = ++p;
    return false;
  }
  char c = '0'; // stopgap for already consumed character
  // integral part
  while (c >= '0' && c <= '9')
    c = (current_ = p) < end_ ? *p++ : '\0';
  // fractional part
  if (c == '.') {
    c = (current_ = p) < end_ ? *p++ : '\0';
    while (c >= '0' && c <= '9')
      c = (current_ = p) < end_ ? *p++ : '\0';
  }
  // exponential part
  if (c == 'e' || c == 'E') {
    c = (current_ = p) < end_ ? *p++ : '\0';
    if (c == '+' || c == '-')
      c = (current_ = p) < end_ ? *p++ : '\0';
    while (c >= '0' && c <= '9')
      c = (current_ = p) < end_ ? *p++ : '\0';
  }
  return true;
}
bool OurReader::readString() {
  Char c = 0;
  while (current_ != end_) {
    c = getNextChar();
    if (c == '\\')
      getNextChar();
    else if (c == '"')
      break;
  }
  return c == '"';
}


bool OurReader::readStringSingleQuote() {
  Char c = 0;
  while (current_ != end_) {
    c = getNextChar();
    if (c == '\\')
      getNextChar();
    else if (c == '\'')
      break;
  }
  return c == '\'';
}

bool OurReader::readObject(Token& tokenStart) {
  Token tokenName;
  JSONCPP_STRING name;
  Value init(objectValue);
  currentValue().swapPayload(init);
  currentValue().setOffsetStart(tokenStart.start_ - begin_);
  while (readToken(tokenName)) {
    bool initialTokenOk = true;
    while (tokenName.type_ == tokenComment && initialTokenOk)
      initialTokenOk = readToken(tokenName);
    if (!initialTokenOk)
      break;
    if (tokenName.type_ == tokenObjectEnd && name.empty()) // empty object
      return true;
    name = "";
    if (tokenName.type_ == tokenString) {
      if (!decodeString(tokenName, name))
        return recoverFromError(tokenObjectEnd);
    } else if (tokenName.type_ == tokenNumber && features_.allowNumericKeys_) {
      Value numberName;
      if (!decodeNumber(tokenName, numberName))
        return recoverFromError(tokenObjectEnd);
      name = numberName.asString();
    } else {
      break;
    }

    Token colon;
    if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
      return addErrorAndRecover(
          "Missing ':' after object member name", colon, tokenObjectEnd);
    }
    if (name.length() >= (1U<<30)) throwRuntimeError("keylength >= 2^30");
    if (features_.rejectDupKeys_ && currentValue().isMember(name)) {
      JSONCPP_STRING msg = "Duplicate key: '" + name + "'";
      return addErrorAndRecover(
          msg, tokenName, tokenObjectEnd);
    }
    Value& value = currentValue()[name];
    nodes_.push(&value);
    bool ok = readValue();
    nodes_.pop();
    if (!ok) // error already set
      return recoverFromError(tokenObjectEnd);

    Token comma;
    if (!readToken(comma) ||
        (comma.type_ != tokenObjectEnd && comma.type_ != tokenArraySeparator &&
         comma.type_ != tokenComment)) {
      return addErrorAndRecover(
          "Missing ',' or '}' in object declaration", comma, tokenObjectEnd);
    }
    bool finalizeTokenOk = true;
    while (comma.type_ == tokenComment && finalizeTokenOk)
      finalizeTokenOk = readToken(comma);
    if (comma.type_ == tokenObjectEnd)
      return true;
  }
  return addErrorAndRecover(
      "Missing '}' or object member name", tokenName, tokenObjectEnd);
}

bool OurReader::readArray(Token& tokenStart) {
  Value init(arrayValue);
  currentValue().swapPayload(init);
  currentValue().setOffsetStart(tokenStart.start_ - begin_);
  skipSpaces();
  if (current_ != end_ && *current_ == ']') // empty array
  {
    Token endArray;
    readToken(endArray);
    return true;
  }
  int index = 0;
  for (;;) {
    Value& value = currentValue()[index++];
    nodes_.push(&value);
    bool ok = readValue();
    nodes_.pop();
    if (!ok) // error already set
      return recoverFromError(tokenArrayEnd);

    Token token;
    // Accept Comment after last item in the array.
    ok = readToken(token);
    while (token.type_ == tokenComment && ok) {
      ok = readToken(token);
    }
    bool badTokenType =
        (token.type_ != tokenArraySeparator && token.type_ != tokenArrayEnd);
    if (!ok || badTokenType) {
      return addErrorAndRecover(
          "Missing ',' or ']' in array declaration", token, tokenArrayEnd);
    }
    if (token.type_ == tokenArrayEnd)
      break;
  }
  return true;
}

bool OurReader::decodeNumber(Token& token) {
  Value decoded;
  if (!decodeNumber(token, decoded))
    return false;
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool OurReader::decodeNumber(Token& token, Value& decoded) {
  // Attempts to parse the number as an integer. If the number is
  // larger than the maximum supported value of an integer then
  // we decode the number as a double.
  Location current = token.start_;
  bool isNegative = *current == '-';
  if (isNegative)
    ++current;
  // TODO: Help the compiler do the div and mod at compile time or get rid of them.
  Value::LargestUInt maxIntegerValue =
      isNegative ? Value::LargestUInt(-Value::minLargestInt)
                 : Value::maxLargestUInt;
  Value::LargestUInt threshold = maxIntegerValue / 10;
  Value::LargestUInt value = 0;
  while (current < token.end_) {
    Char c = *current++;
    if (c < '0' || c > '9')
      return decodeDouble(token, decoded);
    Value::UInt digit(static_cast<Value::UInt>(c - '0'));
    if (value >= threshold) {
      // We've hit or exceeded the max value divided by 10 (rounded down). If
      // a) we've only just touched the limit, b) this is the last digit, and
      // c) it's small enough to fit in that rounding delta, we're okay.
      // Otherwise treat this number as a double to avoid overflow.
      if (value > threshold || current != token.end_ ||
          digit > maxIntegerValue % 10) {
        return decodeDouble(token, decoded);
      }
    }
    value = value * 10 + digit;
  }
  if (isNegative)
    decoded = -Value::LargestInt(value);
  else if (value <= Value::LargestUInt(Value::maxInt))
    decoded = Value::LargestInt(value);
  else
    decoded = value;
  return true;
}

bool OurReader::decodeDouble(Token& token) {
  Value decoded;
  if (!decodeDouble(token, decoded))
    return false;
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool OurReader::decodeDouble(Token& token, Value& decoded) {
  double value = 0;
  const int bufferSize = 32;
  int count;
  ptrdiff_t const length = token.end_ - token.start_;

  // Sanity check to avoid buffer overflow exploits.
  if (length < 0) {
    return addError("Unable to parse token length", token);
  }
  size_t const ulength = static_cast<size_t>(length);

  // Avoid using a string constant for the format control string given to
  // sscanf, as this can cause hard to debug crashes on OS X. See here for more
  // info:
  //
  //     http://developer.apple.com/library/mac/#DOCUMENTATION/DeveloperTools/gcc-4.0.1/gcc/Incompatibilities.html
  char format[] = "%lf";

  if (length <= bufferSize) {
    Char buffer[bufferSize + 1];
    memcpy(buffer, token.start_, ulength);
    buffer[length] = 0;
    fixNumericLocaleInput(buffer, buffer + length);
    count = sscanf(buffer, format, &value);
  } else {
    JSONCPP_STRING buffer(token.start_, token.end_);
    count = sscanf(buffer.c_str(), format, &value);
  }

  if (count != 1)
    return addError("'" + JSONCPP_STRING(token.start_, token.end_) +
                        "' is not a number.",
                    token);
  decoded = value;
  return true;
}

bool OurReader::decodeString(Token& token) {
  JSONCPP_STRING decoded_string;
  if (!decodeString(token, decoded_string))
    return false;
  Value decoded(decoded_string);
  currentValue().swapPayload(decoded);
  currentValue().setOffsetStart(token.start_ - begin_);
  currentValue().setOffsetLimit(token.end_ - begin_);
  return true;
}

bool OurReader::decodeString(Token& token, JSONCPP_STRING& decoded) {
  decoded.reserve(static_cast<size_t>(token.end_ - token.start_ - 2));
  Location current = token.start_ + 1; // skip '"'
  Location end = token.end_ - 1;       // do not include '"'
  while (current != end) {
    Char c = *current++;
    if (c == '"')
      break;
    else if (c == '\\') {
      if (current == end)
        return addError("Empty escape sequence in string", token, current);
      Char escape = *current++;
      switch (escape) {
      case '"':
        decoded += '"';
        break;
      case '/':
        decoded += '/';
        break;
      case '\\':
        decoded += '\\';
        break;
      case 'b':
        decoded += '\b';
        break;
      case 'f':
        decoded += '\f';
        break;
      case 'n':
        decoded += '\n';
        break;
      case 'r':
        decoded += '\r';
        break;
      case 't':
        decoded += '\t';
        break;
      case 'u': {
        unsigned int unicode;
        if (!decodeUnicodeCodePoint(token, current, end, unicode))
          return false;
        decoded += codePointToUTF8(unicode);
      } break;
      default:
        return addError("Bad escape sequence in string", token, current);
      }
    } else {
      decoded += c;
    }
  }
  return true;
}

bool OurReader::decodeUnicodeCodePoint(Token& token,
                                    Location& current,
                                    Location end,
                                    unsigned int& unicode) {

  if (!decodeUnicodeEscapeSequence(token, current, end, unicode))
    return false;
  if (unicode >= 0xD800 && unicode <= 0xDBFF) {
    // surrogate pairs
    if (end - current < 6)
      return addError(
          "additional six characters expected to parse unicode surrogate pair.",
          token,
          current);
    unsigned int surrogatePair;
    if (*(current++) == '\\' && *(current++) == 'u') {
      if (decodeUnicodeEscapeSequence(token, current, end, surrogatePair)) {
        unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
      } else
        return false;
    } else
      return addError("expecting another \\u token to begin the second half of "
                      "a unicode surrogate pair",
                      token,
                      current);
  }
  return true;
}

bool OurReader::decodeUnicodeEscapeSequence(Token& token,
                                         Location& current,
                                         Location end,
                                         unsigned int& ret_unicode) {
  if (end - current < 4)
    return addError(
        "Bad unicode escape sequence in string: four digits expected.",
        token,
        current);
  int unicode = 0;
  for (int index = 0; index < 4; ++index) {
    Char c = *current++;
    unicode *= 16;
    if (c >= '0' && c <= '9')
      unicode += c - '0';
    else if (c >= 'a' && c <= 'f')
      unicode += c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      unicode += c - 'A' + 10;
    else
      return addError(
          "Bad unicode escape sequence in string: hexadecimal digit expected.",
          token,
          current);
  }
  ret_unicode = static_cast<unsigned int>(unicode);
  return true;
}

bool
OurReader::addError(const JSONCPP_STRING& message, Token& token, Location extra) {
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = extra;
  errors_.push_back(info);
  return false;
}

bool OurReader::recoverFromError(TokenType skipUntilToken) {
  size_t errorCount = errors_.size();
  Token skip;
  for (;;) {
    if (!readToken(skip))
      errors_.resize(errorCount); // discard errors caused by recovery
    if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream)
      break;
  }
  errors_.resize(errorCount);
  return false;
}

bool OurReader::addErrorAndRecover(const JSONCPP_STRING& message,
                                Token& token,
                                TokenType skipUntilToken) {
  addError(message, token);
  return recoverFromError(skipUntilToken);
}

Value& OurReader::currentValue() { return *(nodes_.top()); }

OurReader::Char OurReader::getNextChar() {
  if (current_ == end_)
    return 0;
  return *current_++;
}

void OurReader::getLocationLineAndColumn(Location location,
                                      int& line,
                                      int& column) const {
  Location current = begin_;
  Location lastLineStart = current;
  line = 0;
  while (current < location && current != end_) {
    Char c = *current++;
    if (c == '\r') {
      if (*current == '\n')
        ++current;
      lastLineStart = current;
      ++line;
    } else if (c == '\n') {
      lastLineStart = current;
      ++line;
    }
  }
  // column & line start at 1
  column = int(location - lastLineStart) + 1;
  ++line;
}

JSONCPP_STRING OurReader::getLocationLineAndColumn(Location location) const {
  int line, column;
  getLocationLineAndColumn(location, line, column);
  char buffer[18 + 16 + 16 + 1];
  snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
  return buffer;
}

JSONCPP_STRING OurReader::getFormattedErrorMessages() const {
  JSONCPP_STRING formattedMessage;
  for (Errors::const_iterator itError = errors_.begin();
       itError != errors_.end();
       ++itError) {
    const ErrorInfo& error = *itError;
    formattedMessage +=
        "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
    formattedMessage += "  " + error.message_ + "\n";
    if (error.extra_)
      formattedMessage +=
          "See " + getLocationLineAndColumn(error.extra_) + " for detail.\n";
  }
  return formattedMessage;
}

std::vector<OurReader::StructuredError> OurReader::getStructuredErrors() const {
  std::vector<OurReader::StructuredError> allErrors;
  for (Errors::const_iterator itError = errors_.begin();
       itError != errors_.end();
       ++itError) {
    const ErrorInfo& error = *itError;
    OurReader::StructuredError structured;
    structured.offset_start = error.token_.start_ - begin_;
    structured.offset_limit = error.token_.end_ - begin_;
    structured.message = error.message_;
    allErrors.push_back(structured);
  }
  return allErrors;
}

bool OurReader::pushError(const Value& value, const JSONCPP_STRING& message) {
  ptrdiff_t length = end_ - begin_;
  if(value.getOffsetStart() > length
    || value.getOffsetLimit() > length)
    return false;
  Token token;
  token.type_ = tokenError;
  token.start_ = begin_ + value.getOffsetStart();
  token.end_ = end_ + value.getOffsetLimit();
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = 0;
  errors_.push_back(info);
  return true;
}

bool OurReader::pushError(const Value& value, const JSONCPP_STRING& message, const Value& extra) {
  ptrdiff_t length = end_ - begin_;
  if(value.getOffsetStart() > length
    || value.getOffsetLimit() > length
    || extra.getOffsetLimit() > length)
    return false;
  Token token;
  token.type_ = tokenError;
  token.start_ = begin_ + value.getOffsetStart();
  token.end_ = begin_ + value.getOffsetLimit();
  ErrorInfo info;
  info.token_ = token;
  info.message_ = message;
  info.extra_ = begin_ + extra.getOffsetStart();
  errors_.push_back(info);
  return true;
}

bool OurReader::good() const {
  return !errors_.size();
}


class OurCharReader : public CharReader {
  bool const collectComments_;
  OurReader reader_;
public:
  OurCharReader(
    bool collectComments,
    OurFeatures const& features)
  : collectComments_(collectComments)
  , reader_(features)
  {}
  bool parse(
      char const* beginDoc, char const* endDoc,
      Value* root, JSONCPP_STRING* errs) JSONCPP_OVERRIDE {
    bool ok = reader_.parse(beginDoc, endDoc, *root, collectComments_);
    if (errs) {
      *errs = reader_.getFormattedErrorMessages();
    }
    return ok;
  }
};

CharReaderBuilder::CharReaderBuilder()
{
  setDefaults(&settings_);
}
CharReaderBuilder::~CharReaderBuilder()
{}
CharReader* CharReaderBuilder::newCharReader() const
{
  bool collectComments = settings_["collectComments"].asBool();
  OurFeatures features = OurFeatures::all();
  features.allowComments_ = settings_["allowComments"].asBool();
  features.strictRoot_ = settings_["strictRoot"].asBool();
  features.allowDroppedNullPlaceholders_ = settings_["allowDroppedNullPlaceholders"].asBool();
  features.allowNumericKeys_ = settings_["allowNumericKeys"].asBool();
  features.allowSingleQuotes_ = settings_["allowSingleQuotes"].asBool();
  features.stackLimit_ = settings_["stackLimit"].asInt();
  features.failIfExtra_ = settings_["failIfExtra"].asBool();
  features.rejectDupKeys_ = settings_["rejectDupKeys"].asBool();
  features.allowSpecialFloats_ = settings_["allowSpecialFloats"].asBool();
  return new OurCharReader(collectComments, features);
}
static void getValidReaderKeys(std::set<JSONCPP_STRING>* valid_keys)
{
  valid_keys->clear();
  valid_keys->insert("collectComments");
  valid_keys->insert("allowComments");
  valid_keys->insert("strictRoot");
  valid_keys->insert("allowDroppedNullPlaceholders");
  valid_keys->insert("allowNumericKeys");
  valid_keys->insert("allowSingleQuotes");
  valid_keys->insert("stackLimit");
  valid_keys->insert("failIfExtra");
  valid_keys->insert("rejectDupKeys");
  valid_keys->insert("allowSpecialFloats");
}
bool CharReaderBuilder::validate(Json::Value* invalid) const
{
  Json::Value my_invalid;
  if (!invalid) invalid = &my_invalid;  // so we do not need to test for NULL
  Json::Value& inv = *invalid;
  std::set<JSONCPP_STRING> valid_keys;
  getValidReaderKeys(&valid_keys);
  Value::Members keys = settings_.getMemberNames();
  size_t n = keys.size();
  for (size_t i = 0; i < n; ++i) {
    JSONCPP_STRING const& key = keys[i];
    if (valid_keys.find(key) == valid_keys.end()) {
      inv[key] = settings_[key];
    }
  }
  return 0u == inv.size();
}
Value& CharReaderBuilder::operator[](JSONCPP_STRING key)
{
  return settings_[key];
}
// static
void CharReaderBuilder::strictMode(Json::Value* settings)
{
//! [CharReaderBuilderStrictMode]
  (*settings)["allowComments"] = false;
  (*settings)["strictRoot"] = true;
  (*settings)["allowDroppedNullPlaceholders"] = false;
  (*settings)["allowNumericKeys"] = false;
  (*settings)["allowSingleQuotes"] = false;
  (*settings)["stackLimit"] = 1000;
  (*settings)["failIfExtra"] = true;
  (*settings)["rejectDupKeys"] = true;
  (*settings)["allowSpecialFloats"] = false;
//! [CharReaderBuilderStrictMode]
}
// static
void CharReaderBuilder::setDefaults(Json::Value* settings)
{
//! [CharReaderBuilderDefaults]
  (*settings)["collectComments"] = true;
  (*settings)["allowComments"] = true;
  (*settings)["strictRoot"] = false;
  (*settings)["allowDroppedNullPlaceholders"] = false;
  (*settings)["allowNumericKeys"] = false;
  (*settings)["allowSingleQuotes"] = false;
  (*settings)["stackLimit"] = 1000;
  (*settings)["failIfExtra"] = false;
  (*settings)["rejectDupKeys"] = false;
  (*settings)["allowSpecialFloats"] = false;
//! [CharReaderBuilderDefaults]
}

//////////////////////////////////
// global functions

bool parseFromStream(
    CharReader::Factory const& fact, JSONCPP_ISTREAM& sin,
    Value* root, JSONCPP_STRING* errs)
{
  JSONCPP_OSTRINGSTREAM ssin;
  ssin << sin.rdbuf();
  JSONCPP_STRING doc = ssin.str();
  char const* begin = doc.data();
  char const* end = begin + doc.size();
  // Note that we do not actually need a null-terminator.
  CharReaderPtr const reader(fact.newCharReader());
  return reader->parse(begin, end, root, errs);
}

JSONCPP_ISTREAM& operator>>(JSONCPP_ISTREAM& sin, Value& root) {
  CharReaderBuilder b;
  JSONCPP_STRING errs;
  bool ok = parseFromStream(b, sin, &root, &errs);
  if (!ok) {
    fprintf(stderr,
            "Error from reader: %s",
            errs.c_str());

    throwRuntimeError(errs);
  }
  return sin;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_reader.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////

// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

// included by json_value.cpp

namespace Json {

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueIteratorBase
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueIteratorBase::ValueIteratorBase()
    : current_(), isNull_(true) {
}

ValueIteratorBase::ValueIteratorBase(
    const Value::ObjectValues::iterator& current)
    : current_(current), isNull_(false) {}

Value& ValueIteratorBase::deref() const {
  return current_->second;
}

void ValueIteratorBase::increment() {
  ++current_;
}

void ValueIteratorBase::decrement() {
  --current_;
}

ValueIteratorBase::difference_type
ValueIteratorBase::computeDistance(const SelfType& other) const {
#ifdef JSON_USE_CPPTL_SMALLMAP
  return other.current_ - current_;
#else
  // Iterator for null value are initialized using the default
  // constructor, which initialize current_ to the default
  // std::map::iterator. As begin() and end() are two instance
  // of the default std::map::iterator, they can not be compared.
  // To allow this, we handle this comparison specifically.
  if (isNull_ && other.isNull_) {
    return 0;
  }

  // Usage of std::distance is not portable (does not compile with Sun Studio 12
  // RogueWave STL,
  // which is the one used by default).
  // Using a portable hand-made version for non random iterator instead:
  //   return difference_type( std::distance( current_, other.current_ ) );
  difference_type myDistance = 0;
  for (Value::ObjectValues::iterator it = current_; it != other.current_;
       ++it) {
    ++myDistance;
  }
  return myDistance;
#endif
}

bool ValueIteratorBase::isEqual(const SelfType& other) const {
  if (isNull_) {
    return other.isNull_;
  }
  return current_ == other.current_;
}

void ValueIteratorBase::copy(const SelfType& other) {
  current_ = other.current_;
  isNull_ = other.isNull_;
}

Value ValueIteratorBase::key() const {
  const Value::CZString czstring = (*current_).first;
  if (czstring.data()) {
    if (czstring.isStaticString())
      return Value(StaticString(czstring.data()));
    return Value(czstring.data(), czstring.data() + czstring.length());
  }
  return Value(czstring.index());
}

UInt ValueIteratorBase::index() const {
  const Value::CZString czstring = (*current_).first;
  if (!czstring.data())
    return czstring.index();
  return Value::UInt(-1);
}

JSONCPP_STRING ValueIteratorBase::name() const {
  char const* keey;
  char const* end;
  keey = memberName(&end);
  if (!keey) return JSONCPP_STRING();
  return JSONCPP_STRING(keey, end);
}

char const* ValueIteratorBase::memberName() const {
  const char* cname = (*current_).first.data();
  return cname ? cname : "";
}

char const* ValueIteratorBase::memberName(char const** end) const {
  const char* cname = (*current_).first.data();
  if (!cname) {
    *end = NULL;
    return NULL;
  }
  *end = cname + (*current_).first.length();
  return cname;
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueConstIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueConstIterator::ValueConstIterator() {}

ValueConstIterator::ValueConstIterator(
    const Value::ObjectValues::iterator& current)
    : ValueIteratorBase(current) {}

ValueConstIterator::ValueConstIterator(ValueIterator const& other)
    : ValueIteratorBase(other) {}

ValueConstIterator& ValueConstIterator::
operator=(const ValueIteratorBase& other) {
  copy(other);
  return *this;
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class ValueIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

ValueIterator::ValueIterator() {}

ValueIterator::ValueIterator(const Value::ObjectValues::iterator& current)
    : ValueIteratorBase(current) {}

ValueIterator::ValueIterator(const ValueConstIterator& other)
    : ValueIteratorBase(other) {
  throwRuntimeError("ConstIterator to Iterator should never be allowed.");
}

ValueIterator::ValueIterator(const ValueIterator& other)
    : ValueIteratorBase(other) {}

ValueIterator& ValueIterator::operator=(const SelfType& other) {
  copy(other);
  return *this;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_valueiterator.inl
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2011 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/value.h>
#include <json/writer.h>
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <math.h>
#include <sstream>
#include <utility>
#include <cstring>
#include <cassert>
#ifdef JSON_USE_CPPTL
#include <cpptl/conststring.h>
#endif
#include <cstddef> // size_t
#include <algorithm> // min()

#define JSON_ASSERT_UNREACHABLE assert(false)

namespace Json {

// This is a walkaround to avoid the static initialization of Value::null.
// kNull must be word-aligned to avoid crashing on ARM.  We use an alignment of
// 8 (instead of 4) as a bit of future-proofing.
#if defined(__ARMEL__)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#else
#define ALIGNAS(byte_alignment)
#endif
//static const unsigned char ALIGNAS(8) kNull[sizeof(Value)] = { 0 };
//const unsigned char& kNullRef = kNull[0];
//const Value& Value::null = reinterpret_cast<const Value&>(kNullRef);
//const Value& Value::nullRef = null;

// static
Value const& Value::nullSingleton()
{
 static Value const nullStatic;
 return nullStatic;
}

// for backwards compatibility, we'll leave these global references around, but DO NOT
// use them in JSONCPP library code any more!
Value const& Value::null = Value::nullSingleton();
Value const& Value::nullRef = Value::nullSingleton();

const Int Value::minInt = Int(~(UInt(-1) / 2));
const Int Value::maxInt = Int(UInt(-1) / 2);
const UInt Value::maxUInt = UInt(-1);
#if defined(JSON_HAS_INT64)
const Int64 Value::minInt64 = Int64(~(UInt64(-1) / 2));
const Int64 Value::maxInt64 = Int64(UInt64(-1) / 2);
const UInt64 Value::maxUInt64 = UInt64(-1);
// The constant is hard-coded because some compiler have trouble
// converting Value::maxUInt64 to a double correctly (AIX/xlC).
// Assumes that UInt64 is a 64 bits integer.
static const double maxUInt64AsDouble = 18446744073709551615.0;
#endif // defined(JSON_HAS_INT64)
const LargestInt Value::minLargestInt = LargestInt(~(LargestUInt(-1) / 2));
const LargestInt Value::maxLargestInt = LargestInt(LargestUInt(-1) / 2);
const LargestUInt Value::maxLargestUInt = LargestUInt(-1);

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
  // The casts can lose precision, but we are looking only for
  // an approximate range. Might fail on edge cases though. ~cdunn
  //return d >= static_cast<double>(min) && d <= static_cast<double>(max);
  return d >= min && d <= max;
}
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double integerToDouble(Json::UInt64 value) {
  return static_cast<double>(Int64(value / 2)) * 2.0 + static_cast<double>(Int64(value & 1));
}

template <typename T> static inline double integerToDouble(T value) {
  return static_cast<double>(value);
}

template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
  return d >= integerToDouble(min) && d <= integerToDouble(max);
}
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

/** Duplicates the specified string value.
 * @param value Pointer to the string to duplicate. Must be zero-terminated if
 *              length is "unknown".
 * @param length Length of the value. if equals to unknown, then it will be
 *               computed using strlen(value).
 * @return Pointer on the duplicate instance of string.
 */
static inline char* duplicateStringValue(const char* value,
                                         size_t length)
{
  // Avoid an integer overflow in the call to malloc below by limiting length
  // to a sane value.
  if (length >= static_cast<size_t>(Value::maxInt))
    length = Value::maxInt - 1;

  char* newString = static_cast<char*>(malloc(length + 1));
  if (newString == NULL) {
    throwRuntimeError(
        "in Json::Value::duplicateStringValue(): "
        "Failed to allocate string value buffer");
  }
  memcpy(newString, value, length);
  newString[length] = 0;
  return newString;
}

/* Record the length as a prefix.
 */
static inline char* duplicateAndPrefixStringValue(
    const char* value,
    unsigned int length)
{
  // Avoid an integer overflow in the call to malloc below by limiting length
  // to a sane value.
  JSON_ASSERT_MESSAGE(length <= static_cast<unsigned>(Value::maxInt) - sizeof(unsigned) - 1U,
                      "in Json::Value::duplicateAndPrefixStringValue(): "
                      "length too big for prefixing");
  unsigned actualLength = length + static_cast<unsigned>(sizeof(unsigned)) + 1U;
  char* newString = static_cast<char*>(malloc(actualLength));
  if (newString == 0) {
    throwRuntimeError(
        "in Json::Value::duplicateAndPrefixStringValue(): "
        "Failed to allocate string value buffer");
  }
  *reinterpret_cast<unsigned*>(newString) = length;
  memcpy(newString + sizeof(unsigned), value, length);
  newString[actualLength - 1U] = 0; // to avoid buffer over-run accidents by users later
  return newString;
}
inline static void decodePrefixedString(
    bool isPrefixed, char const* prefixed,
    unsigned* length, char const** value)
{
  if (!isPrefixed) {
    *length = static_cast<unsigned>(strlen(prefixed));
    *value = prefixed;
  } else {
    *length = *reinterpret_cast<unsigned const*>(prefixed);
    *value = prefixed + sizeof(unsigned);
  }
}
/** Free the string duplicated by duplicateStringValue()/duplicateAndPrefixStringValue().
 */
#if JSONCPP_USING_SECURE_MEMORY
static inline void releasePrefixedStringValue(char* value) {
  unsigned length = 0;
  char const* valueDecoded;
  decodePrefixedString(true, value, &length, &valueDecoded);
  size_t const size = sizeof(unsigned) + length + 1U;
  memset(value, 0, size);
  free(value);
}
static inline void releaseStringValue(char* value, unsigned length) {
  // length==0 => we allocated the strings memory
  size_t size = (length==0) ? strlen(value) : length;
  memset(value, 0, size);
  free(value);
}
#else // !JSONCPP_USING_SECURE_MEMORY
static inline void releasePrefixedStringValue(char* value) {
  free(value);
}
static inline void releaseStringValue(char* value, unsigned) {
  free(value);
}
#endif // JSONCPP_USING_SECURE_MEMORY

} // namespace Json

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// ValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#if !defined(JSON_IS_AMALGAMATION)

#include "json_valueiterator.inl"
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

Exception::Exception(JSONCPP_STRING const& msg)
  : msg_(msg)
{}
Exception::~Exception() throw()
{}
char const* Exception::what() const throw()
{
  return msg_.c_str();
}
RuntimeError::RuntimeError(JSONCPP_STRING const& msg)
  : Exception(msg)
{}
LogicError::LogicError(JSONCPP_STRING const& msg)
  : Exception(msg)
{}
JSONCPP_NORETURN void throwRuntimeError(JSONCPP_STRING const& msg)
{
  throw RuntimeError(msg);
}
JSONCPP_NORETURN void throwLogicError(JSONCPP_STRING const& msg)
{
  throw LogicError(msg);
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CommentInfo
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

Value::CommentInfo::CommentInfo() : comment_(0)
{}

Value::CommentInfo::~CommentInfo() {
  if (comment_)
    releaseStringValue(comment_, 0u);
}

void Value::CommentInfo::setComment(const char* text, size_t len) {
  if (comment_) {
    releaseStringValue(comment_, 0u);
    comment_ = 0;
  }
  JSON_ASSERT(text != 0);
  JSON_ASSERT_MESSAGE(
      text[0] == '\0' || text[0] == '/',
      "in Json::Value::setComment(): Comments must start with /");
  // It seems that /**/ style comments are acceptable as well.
  comment_ = duplicateStringValue(text, len);
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::CZString
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

// Notes: policy_ indicates if the string was allocated when
// a string is stored.

Value::CZString::CZString(ArrayIndex aindex) : cstr_(0), index_(aindex) {}

Value::CZString::CZString(char const* str, unsigned ulength, DuplicationPolicy allocate)
    : cstr_(str) {
  // allocate != duplicate
  storage_.policy_ = allocate & 0x3;
  storage_.length_ = ulength & 0x3FFFFFFF;
}

Value::CZString::CZString(const CZString& other) {
  cstr_ = (other.storage_.policy_ != noDuplication && other.cstr_ != 0
				 ? duplicateStringValue(other.cstr_, other.storage_.length_)
				 : other.cstr_);
  storage_.policy_ = static_cast<unsigned>(other.cstr_
                 ? (static_cast<DuplicationPolicy>(other.storage_.policy_) == noDuplication
                     ? noDuplication : duplicate)
                 : static_cast<DuplicationPolicy>(other.storage_.policy_)) & 3U;
  storage_.length_ = other.storage_.length_;
}

#if JSON_HAS_RVALUE_REFERENCES
Value::CZString::CZString(CZString&& other)
  : cstr_(other.cstr_), index_(other.index_) {
  other.cstr_ = nullptr;
}
#endif

Value::CZString::~CZString() {
  if (cstr_ && storage_.policy_ == duplicate) {
	  releaseStringValue(const_cast<char*>(cstr_), storage_.length_ + 1u); //+1 for null terminating character for sake of completeness but not actually necessary
  }
}

void Value::CZString::swap(CZString& other) {
  std::swap(cstr_, other.cstr_);
  std::swap(index_, other.index_);
}

Value::CZString& Value::CZString::operator=(CZString other) {
  swap(other);
  return *this;
}

bool Value::CZString::operator<(const CZString& other) const {
  if (!cstr_) return index_ < other.index_;
  //return strcmp(cstr_, other.cstr_) < 0;
  // Assume both are strings.
  unsigned this_len = this->storage_.length_;
  unsigned other_len = other.storage_.length_;
  unsigned min_len = std::min(this_len, other_len);
  JSON_ASSERT(this->cstr_ && other.cstr_);
  int comp = memcmp(this->cstr_, other.cstr_, min_len);
  if (comp < 0) return true;
  if (comp > 0) return false;
  return (this_len < other_len);
}

bool Value::CZString::operator==(const CZString& other) const {
  if (!cstr_) return index_ == other.index_;
  //return strcmp(cstr_, other.cstr_) == 0;
  // Assume both are strings.
  unsigned this_len = this->storage_.length_;
  unsigned other_len = other.storage_.length_;
  if (this_len != other_len) return false;
  JSON_ASSERT(this->cstr_ && other.cstr_);
  int comp = memcmp(this->cstr_, other.cstr_, this_len);
  return comp == 0;
}

ArrayIndex Value::CZString::index() const { return index_; }

//const char* Value::CZString::c_str() const { return cstr_; }
const char* Value::CZString::data() const { return cstr_; }
unsigned Value::CZString::length() const { return storage_.length_; }
bool Value::CZString::isStaticString() const { return storage_.policy_ == noDuplication; }

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class Value::Value
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/*! \internal Default constructor initialization must be equivalent to:
 * memset( this, 0, sizeof(Value) )
 * This optimization is used in ValueInternalMap fast allocator.
 */
Value::Value(ValueType vtype) {
  static char const empty[] = "";
  initBasic(vtype);
  switch (vtype) {
  case nullValue:
    break;
  case intValue:
  case uintValue:
    value_.int_ = 0;
    break;
  case realValue:
    value_.real_ = 0.0;
    break;
  case stringValue:
    // allocated_ == false, so this is safe.
    value_.string_ = const_cast<char*>(static_cast<char const*>(empty));
    break;
  case arrayValue:
  case objectValue:
    value_.map_ = new ObjectValues();
    break;
  case booleanValue:
    value_.bool_ = false;
    break;
  default:
    JSON_ASSERT_UNREACHABLE;
  }
}

Value::Value(Int value) {
  initBasic(intValue);
  value_.int_ = value;
}

Value::Value(UInt value) {
  initBasic(uintValue);
  value_.uint_ = value;
}
#if defined(JSON_HAS_INT64)
Value::Value(Int64 value) {
  initBasic(intValue);
  value_.int_ = value;
}
Value::Value(UInt64 value) {
  initBasic(uintValue);
  value_.uint_ = value;
}
#endif // defined(JSON_HAS_INT64)

Value::Value(double value) {
  initBasic(realValue);
  value_.real_ = value;
}

Value::Value(const char* value) {
  initBasic(stringValue, true);
  value_.string_ = duplicateAndPrefixStringValue(value, static_cast<unsigned>(strlen(value)));
}

Value::Value(const char* beginValue, const char* endValue) {
  initBasic(stringValue, true);
  value_.string_ =
      duplicateAndPrefixStringValue(beginValue, static_cast<unsigned>(endValue - beginValue));
}

Value::Value(const JSONCPP_STRING& value) {
  initBasic(stringValue, true);
  value_.string_ =
      duplicateAndPrefixStringValue(value.data(), static_cast<unsigned>(value.length()));
}

Value::Value(const StaticString& value) {
  initBasic(stringValue);
  value_.string_ = const_cast<char*>(value.c_str());
}

#ifdef JSON_USE_CPPTL
Value::Value(const CppTL::ConstString& value) {
  initBasic(stringValue, true);
  value_.string_ = duplicateAndPrefixStringValue(value, static_cast<unsigned>(value.length()));
}
#endif

Value::Value(bool value) {
  initBasic(booleanValue);
  value_.bool_ = value;
}

Value::Value(Value const& other)
    : type_(other.type_), allocated_(false)
      ,
      comments_(0), start_(other.start_), limit_(other.limit_)
{
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
    value_ = other.value_;
    break;
  case stringValue:
    if (other.value_.string_ && other.allocated_) {
      unsigned len;
      char const* str;
      decodePrefixedString(other.allocated_, other.value_.string_,
          &len, &str);
      value_.string_ = duplicateAndPrefixStringValue(str, len);
      allocated_ = true;
    } else {
      value_.string_ = other.value_.string_;
      allocated_ = false;
    }
    break;
  case arrayValue:
  case objectValue:
    value_.map_ = new ObjectValues(*other.value_.map_);
    break;
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  if (other.comments_) {
    comments_ = new CommentInfo[numberOfCommentPlacement];
    for (int comment = 0; comment < numberOfCommentPlacement; ++comment) {
      const CommentInfo& otherComment = other.comments_[comment];
      if (otherComment.comment_)
        comments_[comment].setComment(
            otherComment.comment_, strlen(otherComment.comment_));
    }
  }
}

#if JSON_HAS_RVALUE_REFERENCES
// Move constructor
Value::Value(Value&& other) {
  initBasic(nullValue);
  swap(other);
}
#endif

Value::~Value() {
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
    break;
  case stringValue:
    if (allocated_)
      releasePrefixedStringValue(value_.string_);
    break;
  case arrayValue:
  case objectValue:
    delete value_.map_;
    break;
  default:
    JSON_ASSERT_UNREACHABLE;
  }

  delete[] comments_;

  value_.uint_ = 0;
}

Value& Value::operator=(Value other) {
  swap(other);
  return *this;
}

void Value::swapPayload(Value& other) {
  ValueType temp = type_;
  type_ = other.type_;
  other.type_ = temp;
  std::swap(value_, other.value_);
  int temp2 = allocated_;
  allocated_ = other.allocated_;
  other.allocated_ = temp2 & 0x1;
}

void Value::swap(Value& other) {
  swapPayload(other);
  std::swap(comments_, other.comments_);
  std::swap(start_, other.start_);
  std::swap(limit_, other.limit_);
}

ValueType Value::type() const { return type_; }

int Value::compare(const Value& other) const {
  if (*this < other)
    return -1;
  if (*this > other)
    return 1;
  return 0;
}

bool Value::operator<(const Value& other) const {
  int typeDelta = type_ - other.type_;
  if (typeDelta)
    return typeDelta < 0 ? true : false;
  switch (type_) {
  case nullValue:
    return false;
  case intValue:
    return value_.int_ < other.value_.int_;
  case uintValue:
    return value_.uint_ < other.value_.uint_;
  case realValue:
    return value_.real_ < other.value_.real_;
  case booleanValue:
    return value_.bool_ < other.value_.bool_;
  case stringValue:
  {
    if ((value_.string_ == 0) || (other.value_.string_ == 0)) {
      if (other.value_.string_) return true;
      else return false;
    }
    unsigned this_len;
    unsigned other_len;
    char const* this_str;
    char const* other_str;
    decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
    decodePrefixedString(other.allocated_, other.value_.string_, &other_len, &other_str);
    unsigned min_len = std::min(this_len, other_len);
    JSON_ASSERT(this_str && other_str);
    int comp = memcmp(this_str, other_str, min_len);
    if (comp < 0) return true;
    if (comp > 0) return false;
    return (this_len < other_len);
  }
  case arrayValue:
  case objectValue: {
    int delta = int(value_.map_->size() - other.value_.map_->size());
    if (delta)
      return delta < 0;
    return (*value_.map_) < (*other.value_.map_);
  }
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  return false; // unreachable
}

bool Value::operator<=(const Value& other) const { return !(other < *this); }

bool Value::operator>=(const Value& other) const { return !(*this < other); }

bool Value::operator>(const Value& other) const { return other < *this; }

bool Value::operator==(const Value& other) const {
  // if ( type_ != other.type_ )
  // GCC 2.95.3 says:
  // attempt to take address of bit-field structure member `Json::Value::type_'
  // Beats me, but a temp solves the problem.
  int temp = other.type_;
  if (type_ != temp)
    return false;
  switch (type_) {
  case nullValue:
    return true;
  case intValue:
    return value_.int_ == other.value_.int_;
  case uintValue:
    return value_.uint_ == other.value_.uint_;
  case realValue:
    return value_.real_ == other.value_.real_;
  case booleanValue:
    return value_.bool_ == other.value_.bool_;
  case stringValue:
  {
    if ((value_.string_ == 0) || (other.value_.string_ == 0)) {
      return (value_.string_ == other.value_.string_);
    }
    unsigned this_len;
    unsigned other_len;
    char const* this_str;
    char const* other_str;
    decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
    decodePrefixedString(other.allocated_, other.value_.string_, &other_len, &other_str);
    if (this_len != other_len) return false;
    JSON_ASSERT(this_str && other_str);
    int comp = memcmp(this_str, other_str, this_len);
    return comp == 0;
  }
  case arrayValue:
  case objectValue:
    return value_.map_->size() == other.value_.map_->size() &&
           (*value_.map_) == (*other.value_.map_);
  default:
    JSON_ASSERT_UNREACHABLE;
  }
  return false; // unreachable
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

const char* Value::asCString() const {
  JSON_ASSERT_MESSAGE(type_ == stringValue,
                      "in Json::Value::asCString(): requires stringValue");
  if (value_.string_ == 0) return 0;
  unsigned this_len;
  char const* this_str;
  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
  return this_str;
}

#if JSONCPP_USING_SECURE_MEMORY
unsigned Value::getCStringLength() const {
  JSON_ASSERT_MESSAGE(type_ == stringValue,
	                  "in Json::Value::asCString(): requires stringValue");
  if (value_.string_ == 0) return 0;
  unsigned this_len;
  char const* this_str;
  decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
  return this_len;
}
#endif

bool Value::getString(char const** str, char const** cend) const {
  if (type_ != stringValue) return false;
  if (value_.string_ == 0) return false;
  unsigned length;
  decodePrefixedString(this->allocated_, this->value_.string_, &length, str);
  *cend = *str + length;
  return true;
}

JSONCPP_STRING Value::asString() const {
  switch (type_) {
  case nullValue:
    return "";
  case stringValue:
  {
    if (value_.string_ == 0) return "";
    unsigned this_len;
    char const* this_str;
    decodePrefixedString(this->allocated_, this->value_.string_, &this_len, &this_str);
    return JSONCPP_STRING(this_str, this_len);
  }
  case booleanValue:
    return value_.bool_ ? "true" : "false";
  case intValue:
    return valueToString(value_.int_);
  case uintValue:
    return valueToString(value_.uint_);
  case realValue:
    return valueToString(value_.real_);
  default:
    JSON_FAIL_MESSAGE("Type is not convertible to string");
  }
}

#ifdef JSON_USE_CPPTL
CppTL::ConstString Value::asConstString() const {
  unsigned len;
  char const* str;
  decodePrefixedString(allocated_, value_.string_,
      &len, &str);
  return CppTL::ConstString(str, len);
}
#endif

Value::Int Value::asInt() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
    return Int(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
    return Int(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
                        "double out of Int range");
    return Int(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to Int.");
}

Value::UInt Value::asUInt() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
    return UInt(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
    return UInt(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
                        "double out of UInt range");
    return UInt(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
}

#if defined(JSON_HAS_INT64)

Value::Int64 Value::asInt64() const {
  switch (type_) {
  case intValue:
    return Int64(value_.int_);
  case uintValue:
    JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
    return Int64(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
                        "double out of Int64 range");
    return Int64(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
}

Value::UInt64 Value::asUInt64() const {
  switch (type_) {
  case intValue:
    JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
    return UInt64(value_.int_);
  case uintValue:
    return UInt64(value_.uint_);
  case realValue:
    JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
                        "double out of UInt64 range");
    return UInt64(value_.real_);
  case nullValue:
    return 0;
  case booleanValue:
    return value_.bool_ ? 1 : 0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
}
#endif // if defined(JSON_HAS_INT64)

LargestInt Value::asLargestInt() const {
#if defined(JSON_NO_INT64)
  return asInt();
#else
  return asInt64();
#endif
}

LargestUInt Value::asLargestUInt() const {
#if defined(JSON_NO_INT64)
  return asUInt();
#else
  return asUInt64();
#endif
}

double Value::asDouble() const {
  switch (type_) {
  case intValue:
    return static_cast<double>(value_.int_);
  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return static_cast<double>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return integerToDouble(value_.uint_);
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
  case realValue:
    return value_.real_;
  case nullValue:
    return 0.0;
  case booleanValue:
    return value_.bool_ ? 1.0 : 0.0;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to double.");
}

float Value::asFloat() const {
  switch (type_) {
  case intValue:
    return static_cast<float>(value_.int_);
  case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    return static_cast<float>(value_.uint_);
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
    // This can fail (silently?) if the value is bigger than MAX_FLOAT.
    return static_cast<float>(integerToDouble(value_.uint_));
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
  case realValue:
    return static_cast<float>(value_.real_);
  case nullValue:
    return 0.0;
  case booleanValue:
    return value_.bool_ ? 1.0f : 0.0f;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to float.");
}

bool Value::asBool() const {
  switch (type_) {
  case booleanValue:
    return value_.bool_;
  case nullValue:
    return false;
  case intValue:
    return value_.int_ ? true : false;
  case uintValue:
    return value_.uint_ ? true : false;
  case realValue:
    // This is kind of strange. Not recommended.
    return (value_.real_ != 0.0) ? true : false;
  default:
    break;
  }
  JSON_FAIL_MESSAGE("Value is not convertible to bool.");
}

bool Value::isConvertibleTo(ValueType other) const {
  switch (other) {
  case nullValue:
    return (isNumeric() && asDouble() == 0.0) ||
           (type_ == booleanValue && value_.bool_ == false) ||
           (type_ == stringValue && asString() == "") ||
           (type_ == arrayValue && value_.map_->size() == 0) ||
           (type_ == objectValue && value_.map_->size() == 0) ||
           type_ == nullValue;
  case intValue:
    return isInt() ||
           (type_ == realValue && InRange(value_.real_, minInt, maxInt)) ||
           type_ == booleanValue || type_ == nullValue;
  case uintValue:
    return isUInt() ||
           (type_ == realValue && InRange(value_.real_, 0, maxUInt)) ||
           type_ == booleanValue || type_ == nullValue;
  case realValue:
    return isNumeric() || type_ == booleanValue || type_ == nullValue;
  case booleanValue:
    return isNumeric() || type_ == booleanValue || type_ == nullValue;
  case stringValue:
    return isNumeric() || type_ == booleanValue || type_ == stringValue ||
           type_ == nullValue;
  case arrayValue:
    return type_ == arrayValue || type_ == nullValue;
  case objectValue:
    return type_ == objectValue || type_ == nullValue;
  }
  JSON_ASSERT_UNREACHABLE;
  return false;
}

/// Number of values in array or object
ArrayIndex Value::size() const {
  switch (type_) {
  case nullValue:
  case intValue:
  case uintValue:
  case realValue:
  case booleanValue:
  case stringValue:
    return 0;
  case arrayValue: // size of the array is highest index + 1
    if (!value_.map_->empty()) {
      ObjectValues::const_iterator itLast = value_.map_->end();
      --itLast;
      return (*itLast).first.index() + 1;
    }
    return 0;
  case objectValue:
    return ArrayIndex(value_.map_->size());
  }
  JSON_ASSERT_UNREACHABLE;
  return 0; // unreachable;
}

bool Value::empty() const {
  if (isNull() || isArray() || isObject())
    return size() == 0u;
  else
    return false;
}

bool Value::operator!() const { return isNull(); }

void Value::clear() {
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue ||
                          type_ == objectValue,
                      "in Json::Value::clear(): requires complex value");
  start_ = 0;
  limit_ = 0;
  switch (type_) {
  case arrayValue:
  case objectValue:
    value_.map_->clear();
    break;
  default:
    break;
  }
}

void Value::resize(ArrayIndex newSize) {
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
                      "in Json::Value::resize(): requires arrayValue");
  if (type_ == nullValue)
    *this = Value(arrayValue);
  ArrayIndex oldSize = size();
  if (newSize == 0)
    clear();
  else if (newSize > oldSize)
    (*this)[newSize - 1];
  else {
    for (ArrayIndex index = newSize; index < oldSize; ++index) {
      value_.map_->erase(index);
    }
    JSON_ASSERT(size() == newSize);
  }
}

Value& Value::operator[](ArrayIndex index) {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == arrayValue,
      "in Json::Value::operator[](ArrayIndex): requires arrayValue");
  if (type_ == nullValue)
    *this = Value(arrayValue);
  CZString key(index);
  ObjectValues::iterator it = value_.map_->lower_bound(key);
  if (it != value_.map_->end() && (*it).first == key)
    return (*it).second;

  ObjectValues::value_type defaultValue(key, nullSingleton());
  it = value_.map_->insert(it, defaultValue);
  return (*it).second;
}

Value& Value::operator[](int index) {
  JSON_ASSERT_MESSAGE(
      index >= 0,
      "in Json::Value::operator[](int index): index cannot be negative");
  return (*this)[ArrayIndex(index)];
}

const Value& Value::operator[](ArrayIndex index) const {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == arrayValue,
      "in Json::Value::operator[](ArrayIndex)const: requires arrayValue");
  if (type_ == nullValue)
    return nullSingleton();
  CZString key(index);
  ObjectValues::const_iterator it = value_.map_->find(key);
  if (it == value_.map_->end())
    return nullSingleton();
  return (*it).second;
}

const Value& Value::operator[](int index) const {
  JSON_ASSERT_MESSAGE(
      index >= 0,
      "in Json::Value::operator[](int index) const: index cannot be negative");
  return (*this)[ArrayIndex(index)];
}

void Value::initBasic(ValueType vtype, bool allocated) {
  type_ = vtype;
  allocated_ = allocated;
  comments_ = 0;
  start_ = 0;
  limit_ = 0;
}

// Access an object value by name, create a null member if it does not exist.
// @pre Type of '*this' is object or null.
// @param key is null-terminated.
Value& Value::resolveReference(const char* key) {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::resolveReference(): requires objectValue");
  if (type_ == nullValue)
    *this = Value(objectValue);
  CZString actualKey(
      key, static_cast<unsigned>(strlen(key)), CZString::noDuplication); // NOTE!
  ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
  if (it != value_.map_->end() && (*it).first == actualKey)
    return (*it).second;

  ObjectValues::value_type defaultValue(actualKey, nullSingleton());
  it = value_.map_->insert(it, defaultValue);
  Value& value = (*it).second;
  return value;
}

// @param key is not null-terminated.
Value& Value::resolveReference(char const* key, char const* cend)
{
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::resolveReference(key, end): requires objectValue");
  if (type_ == nullValue)
    *this = Value(objectValue);
  CZString actualKey(
      key, static_cast<unsigned>(cend-key), CZString::duplicateOnCopy);
  ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
  if (it != value_.map_->end() && (*it).first == actualKey)
    return (*it).second;

  ObjectValues::value_type defaultValue(actualKey, nullSingleton());
  it = value_.map_->insert(it, defaultValue);
  Value& value = (*it).second;
  return value;
}

Value Value::get(ArrayIndex index, const Value& defaultValue) const {
  const Value* value = &((*this)[index]);
  return value == &nullSingleton() ? defaultValue : *value;
}

bool Value::isValidIndex(ArrayIndex index) const { return index < size(); }

Value const* Value::find(char const* key, char const* cend) const
{
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::find(key, end, found): requires objectValue or nullValue");
  if (type_ == nullValue) return NULL;
  CZString actualKey(key, static_cast<unsigned>(cend-key), CZString::noDuplication);
  ObjectValues::const_iterator it = value_.map_->find(actualKey);
  if (it == value_.map_->end()) return NULL;
  return &(*it).second;
}
const Value& Value::operator[](const char* key) const
{
  Value const* found = find(key, key + strlen(key));
  if (!found) return nullSingleton();
  return *found;
}
Value const& Value::operator[](JSONCPP_STRING const& key) const
{
  Value const* found = find(key.data(), key.data() + key.length());
  if (!found) return nullSingleton();
  return *found;
}

Value& Value::operator[](const char* key) {
  return resolveReference(key, key + strlen(key));
}

Value& Value::operator[](const JSONCPP_STRING& key) {
  return resolveReference(key.data(), key.data() + key.length());
}

Value& Value::operator[](const StaticString& key) {
  return resolveReference(key.c_str());
}

#ifdef JSON_USE_CPPTL
Value& Value::operator[](const CppTL::ConstString& key) {
  return resolveReference(key.c_str(), key.end_c_str());
}
Value const& Value::operator[](CppTL::ConstString const& key) const
{
  Value const* found = find(key.c_str(), key.end_c_str());
  if (!found) return nullSingleton();
  return *found;
}
#endif

Value& Value::append(const Value& value) { return (*this)[size()] = value; }

Value Value::get(char const* key, char const* cend, Value const& defaultValue) const
{
  Value const* found = find(key, cend);
  return !found ? defaultValue : *found;
}
Value Value::get(char const* key, Value const& defaultValue) const
{
  return get(key, key + strlen(key), defaultValue);
}
Value Value::get(JSONCPP_STRING const& key, Value const& defaultValue) const
{
  return get(key.data(), key.data() + key.length(), defaultValue);
}


bool Value::removeMember(const char* key, const char* cend, Value* removed)
{
  if (type_ != objectValue) {
    return false;
  }
  CZString actualKey(key, static_cast<unsigned>(cend-key), CZString::noDuplication);
  ObjectValues::iterator it = value_.map_->find(actualKey);
  if (it == value_.map_->end())
    return false;
  *removed = it->second;
  value_.map_->erase(it);
  return true;
}
bool Value::removeMember(const char* key, Value* removed)
{
  return removeMember(key, key + strlen(key), removed);
}
bool Value::removeMember(JSONCPP_STRING const& key, Value* removed)
{
  return removeMember(key.data(), key.data() + key.length(), removed);
}
Value Value::removeMember(const char* key)
{
  JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                      "in Json::Value::removeMember(): requires objectValue");
  if (type_ == nullValue)
    return nullSingleton();

  Value removed;  // null
  removeMember(key, key + strlen(key), &removed);
  return removed; // still null if removeMember() did nothing
}
Value Value::removeMember(const JSONCPP_STRING& key)
{
  return removeMember(key.c_str());
}

bool Value::removeIndex(ArrayIndex index, Value* removed) {
  if (type_ != arrayValue) {
    return false;
  }
  CZString key(index);
  ObjectValues::iterator it = value_.map_->find(key);
  if (it == value_.map_->end()) {
    return false;
  }
  *removed = it->second;
  ArrayIndex oldSize = size();
  // shift left all items left, into the place of the "removed"
  for (ArrayIndex i = index; i < (oldSize - 1); ++i){
    CZString keey(i);
    (*value_.map_)[keey] = (*this)[i + 1];
  }
  // erase the last one ("leftover")
  CZString keyLast(oldSize - 1);
  ObjectValues::iterator itLast = value_.map_->find(keyLast);
  value_.map_->erase(itLast);
  return true;
}

#ifdef JSON_USE_CPPTL
Value Value::get(const CppTL::ConstString& key,
                 const Value& defaultValue) const {
  return get(key.c_str(), key.end_c_str(), defaultValue);
}
#endif

bool Value::isMember(char const* key, char const* cend) const
{
  Value const* value = find(key, cend);
  return NULL != value;
}
bool Value::isMember(char const* key) const
{
  return isMember(key, key + strlen(key));
}
bool Value::isMember(JSONCPP_STRING const& key) const
{
  return isMember(key.data(), key.data() + key.length());
}

#ifdef JSON_USE_CPPTL
bool Value::isMember(const CppTL::ConstString& key) const {
  return isMember(key.c_str(), key.end_c_str());
}
#endif

Value::Members Value::getMemberNames() const {
  JSON_ASSERT_MESSAGE(
      type_ == nullValue || type_ == objectValue,
      "in Json::Value::getMemberNames(), value must be objectValue");
  if (type_ == nullValue)
    return Value::Members();
  Members members;
  members.reserve(value_.map_->size());
  ObjectValues::const_iterator it = value_.map_->begin();
  ObjectValues::const_iterator itEnd = value_.map_->end();
  for (; it != itEnd; ++it) {
    members.push_back(JSONCPP_STRING((*it).first.data(),
                                  (*it).first.length()));
  }
  return members;
}
//
//# ifdef JSON_USE_CPPTL
// EnumMemberNames
// Value::enumMemberNames() const
//{
//   if ( type_ == objectValue )
//   {
//      return CppTL::Enum::any(  CppTL::Enum::transform(
//         CppTL::Enum::keys( *(value_.map_), CppTL::Type<const CZString &>() ),
//         MemberNamesTransform() ) );
//   }
//   return EnumMemberNames();
//}
//
//
// EnumValues
// Value::enumValues() const
//{
//   if ( type_ == objectValue  ||  type_ == arrayValue )
//      return CppTL::Enum::anyValues( *(value_.map_),
//                                     CppTL::Type<const Value &>() );
//   return EnumValues();
//}
//
//# endif

static bool IsIntegral(double d) {
  double integral_part;
  return modf(d, &integral_part) == 0.0;
}

bool Value::isNull() const { return type_ == nullValue; }

bool Value::isBool() const { return type_ == booleanValue; }

bool Value::isInt() const {
  switch (type_) {
  case intValue:
    return value_.int_ >= minInt && value_.int_ <= maxInt;
  case uintValue:
    return value_.uint_ <= UInt(maxInt);
  case realValue:
    return value_.real_ >= minInt && value_.real_ <= maxInt &&
           IsIntegral(value_.real_);
  default:
    break;
  }
  return false;
}

bool Value::isUInt() const {
  switch (type_) {
  case intValue:
    return value_.int_ >= 0 && LargestUInt(value_.int_) <= LargestUInt(maxUInt);
  case uintValue:
    return value_.uint_ <= maxUInt;
  case realValue:
    return value_.real_ >= 0 && value_.real_ <= maxUInt &&
           IsIntegral(value_.real_);
  default:
    break;
  }
  return false;
}

bool Value::isInt64() const {
#if defined(JSON_HAS_INT64)
  switch (type_) {
  case intValue:
    return true;
  case uintValue:
    return value_.uint_ <= UInt64(maxInt64);
  case realValue:
    // Note that maxInt64 (= 2^63 - 1) is not exactly representable as a
    // double, so double(maxInt64) will be rounded up to 2^63. Therefore we
    // require the value to be strictly less than the limit.
    return value_.real_ >= double(minInt64) &&
           value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
  default:
    break;
  }
#endif // JSON_HAS_INT64
  return false;
}

bool Value::isUInt64() const {
#if defined(JSON_HAS_INT64)
  switch (type_) {
  case intValue:
    return value_.int_ >= 0;
  case uintValue:
    return true;
  case realValue:
    // Note that maxUInt64 (= 2^64 - 1) is not exactly representable as a
    // double, so double(maxUInt64) will be rounded up to 2^64. Therefore we
    // require the value to be strictly less than the limit.
    return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble &&
           IsIntegral(value_.real_);
  default:
    break;
  }
#endif // JSON_HAS_INT64
  return false;
}

bool Value::isIntegral() const {
#if defined(JSON_HAS_INT64)
  return isInt64() || isUInt64();
#else
  return isInt() || isUInt();
#endif
}

bool Value::isDouble() const { return type_ == realValue || isIntegral(); }

bool Value::isNumeric() const { return isIntegral() || isDouble(); }

bool Value::isString() const { return type_ == stringValue; }

bool Value::isArray() const { return type_ == arrayValue; }

bool Value::isObject() const { return type_ == objectValue; }

void Value::setComment(const char* comment, size_t len, CommentPlacement placement) {
  if (!comments_)
    comments_ = new CommentInfo[numberOfCommentPlacement];
  if ((len > 0) && (comment[len-1] == '\n')) {
    // Always discard trailing newline, to aid indentation.
    len -= 1;
  }
  comments_[placement].setComment(comment, len);
}

void Value::setComment(const char* comment, CommentPlacement placement) {
  setComment(comment, strlen(comment), placement);
}

void Value::setComment(const JSONCPP_STRING& comment, CommentPlacement placement) {
  setComment(comment.c_str(), comment.length(), placement);
}

bool Value::hasComment(CommentPlacement placement) const {
  return comments_ != 0 && comments_[placement].comment_ != 0;
}

JSONCPP_STRING Value::getComment(CommentPlacement placement) const {
  if (hasComment(placement))
    return comments_[placement].comment_;
  return "";
}

void Value::setOffsetStart(ptrdiff_t start) { start_ = start; }

void Value::setOffsetLimit(ptrdiff_t limit) { limit_ = limit; }

ptrdiff_t Value::getOffsetStart() const { return start_; }

ptrdiff_t Value::getOffsetLimit() const { return limit_; }

JSONCPP_STRING Value::toStyledString() const {
  StyledWriter writer;
  return writer.write(*this);
}

Value::const_iterator Value::begin() const {
  switch (type_) {
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return const_iterator(value_.map_->begin());
    break;
  default:
    break;
  }
  return const_iterator();
}

Value::const_iterator Value::end() const {
  switch (type_) {
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return const_iterator(value_.map_->end());
    break;
  default:
    break;
  }
  return const_iterator();
}

Value::iterator Value::begin() {
  switch (type_) {
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return iterator(value_.map_->begin());
    break;
  default:
    break;
  }
  return iterator();
}

Value::iterator Value::end() {
  switch (type_) {
  case arrayValue:
  case objectValue:
    if (value_.map_)
      return iterator(value_.map_->end());
    break;
  default:
    break;
  }
  return iterator();
}

// class PathArgument
// //////////////////////////////////////////////////////////////////

PathArgument::PathArgument() : key_(), index_(), kind_(kindNone) {}

PathArgument::PathArgument(ArrayIndex index)
    : key_(), index_(index), kind_(kindIndex) {}

PathArgument::PathArgument(const char* key)
    : key_(key), index_(), kind_(kindKey) {}

PathArgument::PathArgument(const JSONCPP_STRING& key)
    : key_(key.c_str()), index_(), kind_(kindKey) {}

// class Path
// //////////////////////////////////////////////////////////////////

Path::Path(const JSONCPP_STRING& path,
           const PathArgument& a1,
           const PathArgument& a2,
           const PathArgument& a3,
           const PathArgument& a4,
           const PathArgument& a5) {
  InArgs in;
  in.push_back(&a1);
  in.push_back(&a2);
  in.push_back(&a3);
  in.push_back(&a4);
  in.push_back(&a5);
  makePath(path, in);
}

void Path::makePath(const JSONCPP_STRING& path, const InArgs& in) {
  const char* current = path.c_str();
  const char* end = current + path.length();
  InArgs::const_iterator itInArg = in.begin();
  while (current != end) {
    if (*current == '[') {
      ++current;
      if (*current == '%')
        addPathInArg(path, in, itInArg, PathArgument::kindIndex);
      else {
        ArrayIndex index = 0;
        for (; current != end && *current >= '0' && *current <= '9'; ++current)
          index = index * 10 + ArrayIndex(*current - '0');
        args_.push_back(index);
      }
      if (current == end || *++current != ']')
        invalidPath(path, int(current - path.c_str()));
    } else if (*current == '%') {
      addPathInArg(path, in, itInArg, PathArgument::kindKey);
      ++current;
    } else if (*current == '.' || *current == ']') {
      ++current;
    } else {
      const char* beginName = current;
      while (current != end && !strchr("[.", *current))
        ++current;
      args_.push_back(JSONCPP_STRING(beginName, current));
    }
  }
}

void Path::addPathInArg(const JSONCPP_STRING& /*path*/,
                        const InArgs& in,
                        InArgs::const_iterator& itInArg,
                        PathArgument::Kind kind) {
  if (itInArg == in.end()) {
    // Error: missing argument %d
  } else if ((*itInArg)->kind_ != kind) {
    // Error: bad argument type
  } else {
    args_.push_back(**itInArg++);
  }
}

void Path::invalidPath(const JSONCPP_STRING& /*path*/, int /*location*/) {
  // Error: invalid path.
}

const Value& Path::resolve(const Value& root) const {
  const Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray() || !node->isValidIndex(arg.index_)) {
        // Error: unable to resolve path (array value expected at position...
        return Value::null;
      }
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject()) {
        // Error: unable to resolve path (object value expected at position...)
        return Value::null;
      }
      node = &((*node)[arg.key_]);
      if (node == &Value::nullSingleton()) {
        // Error: unable to resolve path (object has no member named '' at
        // position...)
        return Value::null;
      }
    }
  }
  return *node;
}

Value Path::resolve(const Value& root, const Value& defaultValue) const {
  const Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray() || !node->isValidIndex(arg.index_))
        return defaultValue;
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject())
        return defaultValue;
      node = &((*node)[arg.key_]);
      if (node == &Value::nullSingleton())
        return defaultValue;
    }
  }
  return *node;
}

Value& Path::make(Value& root) const {
  Value* node = &root;
  for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
    const PathArgument& arg = *it;
    if (arg.kind_ == PathArgument::kindIndex) {
      if (!node->isArray()) {
        // Error: node is not an array at position ...
      }
      node = &((*node)[arg.index_]);
    } else if (arg.kind_ == PathArgument::kindKey) {
      if (!node->isObject()) {
        // Error: node is not an object at position...
      }
      node = &((*node)[arg.key_]);
    }
  }
  return *node;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_value.cpp
// //////////////////////////////////////////////////////////////////////






// //////////////////////////////////////////////////////////////////////
// Beginning of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////

// Copyright 2011 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if !defined(JSON_IS_AMALGAMATION)
#include <json/writer.h>
#include "json_tool.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>
#include <set>
#include <cassert>
#include <cstring>
#include <cstdio>

#if defined(_MSC_VER) && _MSC_VER >= 1200 && _MSC_VER < 1800 // Between VC++ 6.0 and VC++ 11.0
#include <float.h>
#define isfinite _finite
#elif defined(__sun) && defined(__SVR4) //Solaris
#if !defined(isfinite)
#include <ieeefp.h>
#define isfinite finite
#endif
#elif defined(_AIX)
#if !defined(isfinite)
#include <math.h>
#define isfinite finite
#endif
#elif defined(__hpux)
#if !defined(isfinite)
#if defined(__ia64) && !defined(finite)
#define isfinite(x) ((sizeof(x) == sizeof(float) ? \
                     _Isfinitef(x) : _IsFinite(x)))
#else
#include <math.h>
#define isfinite finite
#endif
#endif
#else
#include <cmath>
#if !(defined(__QNXNTO__)) // QNX already defines isfinite
#define isfinite std::isfinite
#endif
#endif

#if defined(_MSC_VER)
#if !defined(WINCE) && defined(__STDC_SECURE_LIB__) && _MSC_VER >= 1500 // VC++ 9.0 and above
#define snprintf sprintf_s
#elif _MSC_VER >= 1900 // VC++ 14.0 and above
#define snprintf std::snprintf
#else
#define snprintf _snprintf
#endif
#elif defined(__ANDROID__) || defined(__QNXNTO__)
#define snprintf snprintf
#elif __cplusplus >= 201103L
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
#define snprintf std::snprintf
#endif
#endif

#if defined(__BORLANDC__)  
#include <float.h>
#define isfinite _finite
#define snprintf _snprintf
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC++ 8.0
// Disable warning about strdup being deprecated.
#pragma warning(disable : 4996)
#endif

namespace Json {

#if __cplusplus >= 201103L || (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
typedef std::unique_ptr<StreamWriter> StreamWriterPtr;
#else
typedef std::auto_ptr<StreamWriter>   StreamWriterPtr;
#endif

static bool containsControlCharacter(const char* str) {
  while (*str) {
    if (isControlCharacter(*(str++)))
      return true;
  }
  return false;
}

static bool containsControlCharacter0(const char* str, unsigned len) {
  char const* end = str + len;
  while (end != str) {
    if (isControlCharacter(*str) || 0==*str)
      return true;
    ++str;
  }
  return false;
}

JSONCPP_STRING valueToString(LargestInt value) {
  UIntToStringBuffer buffer;
  char* current = buffer + sizeof(buffer);
  if (value == Value::minLargestInt) {
    uintToString(LargestUInt(Value::maxLargestInt) + 1, current);
    *--current = '-';
  } else if (value < 0) {
    uintToString(LargestUInt(-value), current);
    *--current = '-';
  } else {
    uintToString(LargestUInt(value), current);
  }
  assert(current >= buffer);
  return current;
}

JSONCPP_STRING valueToString(LargestUInt value) {
  UIntToStringBuffer buffer;
  char* current = buffer + sizeof(buffer);
  uintToString(value, current);
  assert(current >= buffer);
  return current;
}

#if defined(JSON_HAS_INT64)

JSONCPP_STRING valueToString(Int value) {
  return valueToString(LargestInt(value));
}

JSONCPP_STRING valueToString(UInt value) {
  return valueToString(LargestUInt(value));
}

#endif // # if defined(JSON_HAS_INT64)

namespace {
JSONCPP_STRING valueToString(double value, bool useSpecialFloats, unsigned int precision) {
  // Allocate a buffer that is more than large enough to store the 16 digits of
  // precision requested below.
  char buffer[32];
  int len = -1;

  char formatString[6];
  sprintf(formatString, "%%.%dg", precision);

  // Print into the buffer. We need not request the alternative representation
  // that always has a decimal point because JSON doesn't distingish the
  // concepts of reals and integers.
  if (isfinite(value)) {
    len = snprintf(buffer, sizeof(buffer), formatString, value);
  } else {
    // IEEE standard states that NaN values will not compare to themselves
    if (value != value) {
      len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "NaN" : "null");
    } else if (value < 0) {
      len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "-Infinity" : "-1e+9999");
    } else {
      len = snprintf(buffer, sizeof(buffer), useSpecialFloats ? "Infinity" : "1e+9999");
    }
    // For those, we do not need to call fixNumLoc, but it is fast.
  }
  assert(len >= 0);
  fixNumericLocale(buffer, buffer + len);
  return buffer;
}
}

JSONCPP_STRING valueToString(double value) { return valueToString(value, false, 17); }

JSONCPP_STRING valueToString(bool value) { return value ? "true" : "false"; }

JSONCPP_STRING valueToQuotedString(const char* value) {
  if (value == NULL)
    return "";
  // Not sure how to handle unicode...
  if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL &&
      !containsControlCharacter(value))
    return JSONCPP_STRING("\"") + value + "\"";
  // We have to walk value and escape any special characters.
  // Appending to JSONCPP_STRING is not efficient, but this should be rare.
  // (Note: forward slashes are *not* rare, but I am not escaping them.)
  JSONCPP_STRING::size_type maxsize =
      strlen(value) * 2 + 3; // allescaped+quotes+NULL
  JSONCPP_STRING result;
  result.reserve(maxsize); // to avoid lots of mallocs
  result += "\"";
  for (const char* c = value; *c != 0; ++c) {
    switch (*c) {
    case '\"':
      result += "\\\"";
      break;
    case '\\':
      result += "\\\\";
      break;
    case '\b':
      result += "\\b";
      break;
    case '\f':
      result += "\\f";
      break;
    case '\n':
      result += "\\n";
      break;
    case '\r':
      result += "\\r";
      break;
    case '\t':
      result += "\\t";
      break;
    // case '/':
    // Even though \/ is considered a legal escape in JSON, a bare
    // slash is also legal, so I see no reason to escape it.
    // (I hope I am not misunderstanding something.
    // blep notes: actually escaping \/ may be useful in javascript to avoid </
    // sequence.
    // Should add a flag to allow this compatibility mode and prevent this
    // sequence from occurring.
    default:
      if (isControlCharacter(*c)) {
        JSONCPP_OSTRINGSTREAM oss;
        oss << "\\u" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(4) << static_cast<int>(*c);
        result += oss.str();
      } else {
        result += *c;
      }
      break;
    }
  }
  result += "\"";
  return result;
}

// https://github.com/upcaste/upcaste/blob/master/src/upcore/src/cstring/strnpbrk.cpp
static char const* strnpbrk(char const* s, char const* accept, size_t n) {
  assert((s || !n) && accept);

  char const* const end = s + n;
  for (char const* cur = s; cur < end; ++cur) {
    int const c = *cur;
    for (char const* a = accept; *a; ++a) {
      if (*a == c) {
        return cur;
      }
    }
  }
  return NULL;
}
static JSONCPP_STRING valueToQuotedStringN(const char* value, unsigned length) {
  if (value == NULL)
    return "";
  // Not sure how to handle unicode...
  if (strnpbrk(value, "\"\\\b\f\n\r\t", length) == NULL &&
      !containsControlCharacter0(value, length))
    return JSONCPP_STRING("\"") + value + "\"";
  // We have to walk value and escape any special characters.
  // Appending to JSONCPP_STRING is not efficient, but this should be rare.
  // (Note: forward slashes are *not* rare, but I am not escaping them.)
  JSONCPP_STRING::size_type maxsize =
      length * 2 + 3; // allescaped+quotes+NULL
  JSONCPP_STRING result;
  result.reserve(maxsize); // to avoid lots of mallocs
  result += "\"";
  char const* end = value + length;
  for (const char* c = value; c != end; ++c) {
    switch (*c) {
    case '\"':
      result += "\\\"";
      break;
    case '\\':
      result += "\\\\";
      break;
    case '\b':
      result += "\\b";
      break;
    case '\f':
      result += "\\f";
      break;
    case '\n':
      result += "\\n";
      break;
    case '\r':
      result += "\\r";
      break;
    case '\t':
      result += "\\t";
      break;
    // case '/':
    // Even though \/ is considered a legal escape in JSON, a bare
    // slash is also legal, so I see no reason to escape it.
    // (I hope I am not misunderstanding something.)
    // blep notes: actually escaping \/ may be useful in javascript to avoid </
    // sequence.
    // Should add a flag to allow this compatibility mode and prevent this
    // sequence from occurring.
    default:
      if ((isControlCharacter(*c)) || (*c == 0)) {
        JSONCPP_OSTRINGSTREAM oss;
        oss << "\\u" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(4) << static_cast<int>(*c);
        result += oss.str();
      } else {
        result += *c;
      }
      break;
    }
  }
  result += "\"";
  return result;
}

// Class Writer
// //////////////////////////////////////////////////////////////////
Writer::~Writer() {}

// Class FastWriter
// //////////////////////////////////////////////////////////////////

FastWriter::FastWriter()
    : yamlCompatiblityEnabled_(false), dropNullPlaceholders_(false),
      omitEndingLineFeed_(false) {}

void FastWriter::enableYAMLCompatibility() { yamlCompatiblityEnabled_ = true; }

void FastWriter::dropNullPlaceholders() { dropNullPlaceholders_ = true; }

void FastWriter::omitEndingLineFeed() { omitEndingLineFeed_ = true; }

JSONCPP_STRING FastWriter::write(const Value& root) {
  document_ = "";
  writeValue(root);
  if (!omitEndingLineFeed_)
    document_ += "\n";
  return document_;
}

void FastWriter::writeValue(const Value& value) {
  switch (value.type()) {
  case nullValue:
    if (!dropNullPlaceholders_)
      document_ += "null";
    break;
  case intValue:
    document_ += valueToString(value.asLargestInt());
    break;
  case uintValue:
    document_ += valueToString(value.asLargestUInt());
    break;
  case realValue:
    document_ += valueToString(value.asDouble());
    break;
  case stringValue:
  {
    // Is NULL possible for value.string_? No.
    char const* str;
    char const* end;
    bool ok = value.getString(&str, &end);
    if (ok) document_ += valueToQuotedStringN(str, static_cast<unsigned>(end-str));
    break;
  }
  case booleanValue:
    document_ += valueToString(value.asBool());
    break;
  case arrayValue: {
    document_ += '[';
    ArrayIndex size = value.size();
    for (ArrayIndex index = 0; index < size; ++index) {
      if (index > 0)
        document_ += ',';
      writeValue(value[index]);
    }
    document_ += ']';
  } break;
  case objectValue: {
    Value::Members members(value.getMemberNames());
    document_ += '{';
    for (Value::Members::iterator it = members.begin(); it != members.end();
         ++it) {
      const JSONCPP_STRING& name = *it;
      if (it != members.begin())
        document_ += ',';
      document_ += valueToQuotedStringN(name.data(), static_cast<unsigned>(name.length()));
      document_ += yamlCompatiblityEnabled_ ? ": " : ":";
      writeValue(value[name]);
    }
    document_ += '}';
  } break;
  }
}

// Class StyledWriter
// //////////////////////////////////////////////////////////////////

StyledWriter::StyledWriter()
    : rightMargin_(74), indentSize_(3), addChildValues_() {}

JSONCPP_STRING StyledWriter::write(const Value& root) {
  document_ = "";
  addChildValues_ = false;
  indentString_ = "";
  writeCommentBeforeValue(root);
  writeValue(root);
  writeCommentAfterValueOnSameLine(root);
  document_ += "\n";
  return document_;
}

void StyledWriter::writeValue(const Value& value) {
  switch (value.type()) {
  case nullValue:
    pushValue("null");
    break;
  case intValue:
    pushValue(valueToString(value.asLargestInt()));
    break;
  case uintValue:
    pushValue(valueToString(value.asLargestUInt()));
    break;
  case realValue:
    pushValue(valueToString(value.asDouble()));
    break;
  case stringValue:
  {
    // Is NULL possible for value.string_? No.
    char const* str;
    char const* end;
    bool ok = value.getString(&str, &end);
    if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end-str)));
    else pushValue("");
    break;
  }
  case booleanValue:
    pushValue(valueToString(value.asBool()));
    break;
  case arrayValue:
    writeArrayValue(value);
    break;
  case objectValue: {
    Value::Members members(value.getMemberNames());
    if (members.empty())
      pushValue("{}");
    else {
      writeWithIndent("{");
      indent();
      Value::Members::iterator it = members.begin();
      for (;;) {
        const JSONCPP_STRING& name = *it;
        const Value& childValue = value[name];
        writeCommentBeforeValue(childValue);
        writeWithIndent(valueToQuotedString(name.c_str()));
        document_ += " : ";
        writeValue(childValue);
        if (++it == members.end()) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        document_ += ',';
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("}");
    }
  } break;
  }
}

void StyledWriter::writeArrayValue(const Value& value) {
  unsigned size = value.size();
  if (size == 0)
    pushValue("[]");
  else {
    bool isArrayMultiLine = isMultineArray(value);
    if (isArrayMultiLine) {
      writeWithIndent("[");
      indent();
      bool hasChildValue = !childValues_.empty();
      unsigned index = 0;
      for (;;) {
        const Value& childValue = value[index];
        writeCommentBeforeValue(childValue);
        if (hasChildValue)
          writeWithIndent(childValues_[index]);
        else {
          writeIndent();
          writeValue(childValue);
        }
        if (++index == size) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        document_ += ',';
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("]");
    } else // output on a single line
    {
      assert(childValues_.size() == size);
      document_ += "[ ";
      for (unsigned index = 0; index < size; ++index) {
        if (index > 0)
          document_ += ", ";
        document_ += childValues_[index];
      }
      document_ += " ]";
    }
  }
}

bool StyledWriter::isMultineArray(const Value& value) {
  ArrayIndex const size = value.size();
  bool isMultiLine = size * 3 >= rightMargin_;
  childValues_.clear();
  for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
    const Value& childValue = value[index];
    isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                        childValue.size() > 0);
  }
  if (!isMultiLine) // check if line length > max line length
  {
    childValues_.reserve(size);
    addChildValues_ = true;
    ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
    for (ArrayIndex index = 0; index < size; ++index) {
      if (hasCommentForValue(value[index])) {
        isMultiLine = true;
      }
      writeValue(value[index]);
      lineLength += static_cast<ArrayIndex>(childValues_[index].length());
    }
    addChildValues_ = false;
    isMultiLine = isMultiLine || lineLength >= rightMargin_;
  }
  return isMultiLine;
}

void StyledWriter::pushValue(const JSONCPP_STRING& value) {
  if (addChildValues_)
    childValues_.push_back(value);
  else
    document_ += value;
}

void StyledWriter::writeIndent() {
  if (!document_.empty()) {
    char last = document_[document_.length() - 1];
    if (last == ' ') // already indented
      return;
    if (last != '\n') // Comments may add new-line
      document_ += '\n';
  }
  document_ += indentString_;
}

void StyledWriter::writeWithIndent(const JSONCPP_STRING& value) {
  writeIndent();
  document_ += value;
}

void StyledWriter::indent() { indentString_ += JSONCPP_STRING(indentSize_, ' '); }

void StyledWriter::unindent() {
  assert(indentString_.size() >= indentSize_);
  indentString_.resize(indentString_.size() - indentSize_);
}

void StyledWriter::writeCommentBeforeValue(const Value& root) {
  if (!root.hasComment(commentBefore))
    return;

  document_ += "\n";
  writeIndent();
  const JSONCPP_STRING& comment = root.getComment(commentBefore);
  JSONCPP_STRING::const_iterator iter = comment.begin();
  while (iter != comment.end()) {
    document_ += *iter;
    if (*iter == '\n' &&
       (iter != comment.end() && *(iter + 1) == '/'))
      writeIndent();
    ++iter;
  }

  // Comments are stripped of trailing newlines, so add one here
  document_ += "\n";
}

void StyledWriter::writeCommentAfterValueOnSameLine(const Value& root) {
  if (root.hasComment(commentAfterOnSameLine))
    document_ += " " + root.getComment(commentAfterOnSameLine);

  if (root.hasComment(commentAfter)) {
    document_ += "\n";
    document_ += root.getComment(commentAfter);
    document_ += "\n";
  }
}

bool StyledWriter::hasCommentForValue(const Value& value) {
  return value.hasComment(commentBefore) ||
         value.hasComment(commentAfterOnSameLine) ||
         value.hasComment(commentAfter);
}

// Class StyledStreamWriter
// //////////////////////////////////////////////////////////////////

StyledStreamWriter::StyledStreamWriter(JSONCPP_STRING indentation)
    : document_(NULL), rightMargin_(74), indentation_(indentation),
      addChildValues_() {}

void StyledStreamWriter::write(JSONCPP_OSTREAM& out, const Value& root) {
  document_ = &out;
  addChildValues_ = false;
  indentString_ = "";
  indented_ = true;
  writeCommentBeforeValue(root);
  if (!indented_) writeIndent();
  indented_ = true;
  writeValue(root);
  writeCommentAfterValueOnSameLine(root);
  *document_ << "\n";
  document_ = NULL; // Forget the stream, for safety.
}

void StyledStreamWriter::writeValue(const Value& value) {
  switch (value.type()) {
  case nullValue:
    pushValue("null");
    break;
  case intValue:
    pushValue(valueToString(value.asLargestInt()));
    break;
  case uintValue:
    pushValue(valueToString(value.asLargestUInt()));
    break;
  case realValue:
    pushValue(valueToString(value.asDouble()));
    break;
  case stringValue:
  {
    // Is NULL possible for value.string_? No.
    char const* str;
    char const* end;
    bool ok = value.getString(&str, &end);
    if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end-str)));
    else pushValue("");
    break;
  }
  case booleanValue:
    pushValue(valueToString(value.asBool()));
    break;
  case arrayValue:
    writeArrayValue(value);
    break;
  case objectValue: {
    Value::Members members(value.getMemberNames());
    if (members.empty())
      pushValue("{}");
    else {
      writeWithIndent("{");
      indent();
      Value::Members::iterator it = members.begin();
      for (;;) {
        const JSONCPP_STRING& name = *it;
        const Value& childValue = value[name];
        writeCommentBeforeValue(childValue);
        writeWithIndent(valueToQuotedString(name.c_str()));
        *document_ << " : ";
        writeValue(childValue);
        if (++it == members.end()) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        *document_ << ",";
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("}");
    }
  } break;
  }
}

void StyledStreamWriter::writeArrayValue(const Value& value) {
  unsigned size = value.size();
  if (size == 0)
    pushValue("[]");
  else {
    bool isArrayMultiLine = isMultineArray(value);
    if (isArrayMultiLine) {
      writeWithIndent("[");
      indent();
      bool hasChildValue = !childValues_.empty();
      unsigned index = 0;
      for (;;) {
        const Value& childValue = value[index];
        writeCommentBeforeValue(childValue);
        if (hasChildValue)
          writeWithIndent(childValues_[index]);
        else {
          if (!indented_) writeIndent();
          indented_ = true;
          writeValue(childValue);
          indented_ = false;
        }
        if (++index == size) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        *document_ << ",";
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("]");
    } else // output on a single line
    {
      assert(childValues_.size() == size);
      *document_ << "[ ";
      for (unsigned index = 0; index < size; ++index) {
        if (index > 0)
          *document_ << ", ";
        *document_ << childValues_[index];
      }
      *document_ << " ]";
    }
  }
}

bool StyledStreamWriter::isMultineArray(const Value& value) {
  ArrayIndex const size = value.size();
  bool isMultiLine = size * 3 >= rightMargin_;
  childValues_.clear();
  for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
    const Value& childValue = value[index];
    isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                        childValue.size() > 0);
  }
  if (!isMultiLine) // check if line length > max line length
  {
    childValues_.reserve(size);
    addChildValues_ = true;
    ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
    for (ArrayIndex index = 0; index < size; ++index) {
      if (hasCommentForValue(value[index])) {
        isMultiLine = true;
      }
      writeValue(value[index]);
      lineLength += static_cast<ArrayIndex>(childValues_[index].length());
    }
    addChildValues_ = false;
    isMultiLine = isMultiLine || lineLength >= rightMargin_;
  }
  return isMultiLine;
}

void StyledStreamWriter::pushValue(const JSONCPP_STRING& value) {
  if (addChildValues_)
    childValues_.push_back(value);
  else
    *document_ << value;
}

void StyledStreamWriter::writeIndent() {
  // blep intended this to look at the so-far-written string
  // to determine whether we are already indented, but
  // with a stream we cannot do that. So we rely on some saved state.
  // The caller checks indented_.
  *document_ << '\n' << indentString_;
}

void StyledStreamWriter::writeWithIndent(const JSONCPP_STRING& value) {
  if (!indented_) writeIndent();
  *document_ << value;
  indented_ = false;
}

void StyledStreamWriter::indent() { indentString_ += indentation_; }

void StyledStreamWriter::unindent() {
  assert(indentString_.size() >= indentation_.size());
  indentString_.resize(indentString_.size() - indentation_.size());
}

void StyledStreamWriter::writeCommentBeforeValue(const Value& root) {
  if (!root.hasComment(commentBefore))
    return;

  if (!indented_) writeIndent();
  const JSONCPP_STRING& comment = root.getComment(commentBefore);
  JSONCPP_STRING::const_iterator iter = comment.begin();
  while (iter != comment.end()) {
    *document_ << *iter;
    if (*iter == '\n' &&
       (iter != comment.end() && *(iter + 1) == '/'))
      // writeIndent();  // would include newline
      *document_ << indentString_;
    ++iter;
  }
  indented_ = false;
}

void StyledStreamWriter::writeCommentAfterValueOnSameLine(const Value& root) {
  if (root.hasComment(commentAfterOnSameLine))
    *document_ << ' ' << root.getComment(commentAfterOnSameLine);

  if (root.hasComment(commentAfter)) {
    writeIndent();
    *document_ << root.getComment(commentAfter);
  }
  indented_ = false;
}

bool StyledStreamWriter::hasCommentForValue(const Value& value) {
  return value.hasComment(commentBefore) ||
         value.hasComment(commentAfterOnSameLine) ||
         value.hasComment(commentAfter);
}

//////////////////////////
// BuiltStyledStreamWriter

/// Scoped enums are not available until C++11.
struct CommentStyle {
  /// Decide whether to write comments.
  enum Enum {
    None,  ///< Drop all comments.
    Most,  ///< Recover odd behavior of previous versions (not implemented yet).
    All  ///< Keep all comments.
  };
};

struct BuiltStyledStreamWriter : public StreamWriter
{
  BuiltStyledStreamWriter(
      JSONCPP_STRING const& indentation,
      CommentStyle::Enum cs,
      JSONCPP_STRING const& colonSymbol,
      JSONCPP_STRING const& nullSymbol,
      JSONCPP_STRING const& endingLineFeedSymbol,
      bool useSpecialFloats,
      unsigned int precision);
  int write(Value const& root, JSONCPP_OSTREAM* sout) JSONCPP_OVERRIDE;
private:
  void writeValue(Value const& value);
  void writeArrayValue(Value const& value);
  bool isMultineArray(Value const& value);
  void pushValue(JSONCPP_STRING const& value);
  void writeIndent();
  void writeWithIndent(JSONCPP_STRING const& value);
  void indent();
  void unindent();
  void writeCommentBeforeValue(Value const& root);
  void writeCommentAfterValueOnSameLine(Value const& root);
  static bool hasCommentForValue(const Value& value);

  typedef std::vector<JSONCPP_STRING> ChildValues;

  ChildValues childValues_;
  JSONCPP_STRING indentString_;
  unsigned int rightMargin_;
  JSONCPP_STRING indentation_;
  CommentStyle::Enum cs_;
  JSONCPP_STRING colonSymbol_;
  JSONCPP_STRING nullSymbol_;
  JSONCPP_STRING endingLineFeedSymbol_;
  bool addChildValues_ : 1;
  bool indented_ : 1;
  bool useSpecialFloats_ : 1;
  unsigned int precision_;
};
BuiltStyledStreamWriter::BuiltStyledStreamWriter(
      JSONCPP_STRING const& indentation,
      CommentStyle::Enum cs,
      JSONCPP_STRING const& colonSymbol,
      JSONCPP_STRING const& nullSymbol,
      JSONCPP_STRING const& endingLineFeedSymbol,
      bool useSpecialFloats,
      unsigned int precision)
  : rightMargin_(74)
  , indentation_(indentation)
  , cs_(cs)
  , colonSymbol_(colonSymbol)
  , nullSymbol_(nullSymbol)
  , endingLineFeedSymbol_(endingLineFeedSymbol)
  , addChildValues_(false)
  , indented_(false)
  , useSpecialFloats_(useSpecialFloats)
  , precision_(precision)
{
}
int BuiltStyledStreamWriter::write(Value const& root, JSONCPP_OSTREAM* sout)
{
  sout_ = sout;
  addChildValues_ = false;
  indented_ = true;
  indentString_ = "";
  writeCommentBeforeValue(root);
  if (!indented_) writeIndent();
  indented_ = true;
  writeValue(root);
  writeCommentAfterValueOnSameLine(root);
  *sout_ << endingLineFeedSymbol_;
  sout_ = NULL;
  return 0;
}
void BuiltStyledStreamWriter::writeValue(Value const& value) {
  switch (value.type()) {
  case nullValue:
    pushValue(nullSymbol_);
    break;
  case intValue:
    pushValue(valueToString(value.asLargestInt()));
    break;
  case uintValue:
    pushValue(valueToString(value.asLargestUInt()));
    break;
  case realValue:
    pushValue(valueToString(value.asDouble(), useSpecialFloats_, precision_));
    break;
  case stringValue:
  {
    // Is NULL is possible for value.string_? No.
    char const* str;
    char const* end;
    bool ok = value.getString(&str, &end);
    if (ok) pushValue(valueToQuotedStringN(str, static_cast<unsigned>(end-str)));
    else pushValue("");
    break;
  }
  case booleanValue:
    pushValue(valueToString(value.asBool()));
    break;
  case arrayValue:
    writeArrayValue(value);
    break;
  case objectValue: {
    Value::Members members(value.getMemberNames());
    if (members.empty())
      pushValue("{}");
    else {
      writeWithIndent("{");
      indent();
      Value::Members::iterator it = members.begin();
      for (;;) {
        JSONCPP_STRING const& name = *it;
        Value const& childValue = value[name];
        writeCommentBeforeValue(childValue);
        writeWithIndent(valueToQuotedStringN(name.data(), static_cast<unsigned>(name.length())));
        *sout_ << colonSymbol_;
        writeValue(childValue);
        if (++it == members.end()) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        *sout_ << ",";
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("}");
    }
  } break;
  }
}

void BuiltStyledStreamWriter::writeArrayValue(Value const& value) {
  unsigned size = value.size();
  if (size == 0)
    pushValue("[]");
  else {
    bool isMultiLine = (cs_ == CommentStyle::All) || isMultineArray(value);
    if (isMultiLine) {
      writeWithIndent("[");
      indent();
      bool hasChildValue = !childValues_.empty();
      unsigned index = 0;
      for (;;) {
        Value const& childValue = value[index];
        writeCommentBeforeValue(childValue);
        if (hasChildValue)
          writeWithIndent(childValues_[index]);
        else {
          if (!indented_) writeIndent();
          indented_ = true;
          writeValue(childValue);
          indented_ = false;
        }
        if (++index == size) {
          writeCommentAfterValueOnSameLine(childValue);
          break;
        }
        *sout_ << ",";
        writeCommentAfterValueOnSameLine(childValue);
      }
      unindent();
      writeWithIndent("]");
    } else // output on a single line
    {
      assert(childValues_.size() == size);
      *sout_ << "[";
      if (!indentation_.empty()) *sout_ << " ";
      for (unsigned index = 0; index < size; ++index) {
        if (index > 0)
          *sout_ << ((!indentation_.empty()) ? ", " : ",");
        *sout_ << childValues_[index];
      }
      if (!indentation_.empty()) *sout_ << " ";
      *sout_ << "]";
    }
  }
}

bool BuiltStyledStreamWriter::isMultineArray(Value const& value) {
  ArrayIndex const size = value.size();
  bool isMultiLine = size * 3 >= rightMargin_;
  childValues_.clear();
  for (ArrayIndex index = 0; index < size && !isMultiLine; ++index) {
    Value const& childValue = value[index];
    isMultiLine = ((childValue.isArray() || childValue.isObject()) &&
                        childValue.size() > 0);
  }
  if (!isMultiLine) // check if line length > max line length
  {
    childValues_.reserve(size);
    addChildValues_ = true;
    ArrayIndex lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
    for (ArrayIndex index = 0; index < size; ++index) {
      if (hasCommentForValue(value[index])) {
        isMultiLine = true;
      }
      writeValue(value[index]);
      lineLength += static_cast<ArrayIndex>(childValues_[index].length());
    }
    addChildValues_ = false;
    isMultiLine = isMultiLine || lineLength >= rightMargin_;
  }
  return isMultiLine;
}

void BuiltStyledStreamWriter::pushValue(JSONCPP_STRING const& value) {
  if (addChildValues_)
    childValues_.push_back(value);
  else
    *sout_ << value;
}

void BuiltStyledStreamWriter::writeIndent() {
  // blep intended this to look at the so-far-written string
  // to determine whether we are already indented, but
  // with a stream we cannot do that. So we rely on some saved state.
  // The caller checks indented_.

  if (!indentation_.empty()) {
    // In this case, drop newlines too.
    *sout_ << '\n' << indentString_;
  }
}

void BuiltStyledStreamWriter::writeWithIndent(JSONCPP_STRING const& value) {
  if (!indented_) writeIndent();
  *sout_ << value;
  indented_ = false;
}

void BuiltStyledStreamWriter::indent() { indentString_ += indentation_; }

void BuiltStyledStreamWriter::unindent() {
  assert(indentString_.size() >= indentation_.size());
  indentString_.resize(indentString_.size() - indentation_.size());
}

void BuiltStyledStreamWriter::writeCommentBeforeValue(Value const& root) {
  if (cs_ == CommentStyle::None) return;
  if (!root.hasComment(commentBefore))
    return;

  if (!indented_) writeIndent();
  const JSONCPP_STRING& comment = root.getComment(commentBefore);
  JSONCPP_STRING::const_iterator iter = comment.begin();
  while (iter != comment.end()) {
    *sout_ << *iter;
    if (*iter == '\n' &&
       (iter != comment.end() && *(iter + 1) == '/'))
      // writeIndent();  // would write extra newline
      *sout_ << indentString_;
    ++iter;
  }
  indented_ = false;
}

void BuiltStyledStreamWriter::writeCommentAfterValueOnSameLine(Value const& root) {
  if (cs_ == CommentStyle::None) return;
  if (root.hasComment(commentAfterOnSameLine))
    *sout_ << " " + root.getComment(commentAfterOnSameLine);

  if (root.hasComment(commentAfter)) {
    writeIndent();
    *sout_ << root.getComment(commentAfter);
  }
}

// static
bool BuiltStyledStreamWriter::hasCommentForValue(const Value& value) {
  return value.hasComment(commentBefore) ||
         value.hasComment(commentAfterOnSameLine) ||
         value.hasComment(commentAfter);
}

///////////////
// StreamWriter

StreamWriter::StreamWriter()
    : sout_(NULL)
{
}
StreamWriter::~StreamWriter()
{
}
StreamWriter::Factory::~Factory()
{}
StreamWriterBuilder::StreamWriterBuilder()
{
  setDefaults(&settings_);
}
StreamWriterBuilder::~StreamWriterBuilder()
{}
StreamWriter* StreamWriterBuilder::newStreamWriter() const
{
  JSONCPP_STRING indentation = settings_["indentation"].asString();
  JSONCPP_STRING cs_str = settings_["commentStyle"].asString();
  bool eyc = settings_["enableYAMLCompatibility"].asBool();
  bool dnp = settings_["dropNullPlaceholders"].asBool();
  bool usf = settings_["useSpecialFloats"].asBool(); 
  unsigned int pre = settings_["precision"].asUInt();
  CommentStyle::Enum cs = CommentStyle::All;
  if (cs_str == "All") {
    cs = CommentStyle::All;
  } else if (cs_str == "None") {
    cs = CommentStyle::None;
  } else {
    throwRuntimeError("commentStyle must be 'All' or 'None'");
  }
  JSONCPP_STRING colonSymbol = " : ";
  if (eyc) {
    colonSymbol = ": ";
  } else if (indentation.empty()) {
    colonSymbol = ":";
  }
  JSONCPP_STRING nullSymbol = "null";
  if (dnp) {
    nullSymbol = "";
  }
  if (pre > 17) pre = 17;
  JSONCPP_STRING endingLineFeedSymbol = "";
  return new BuiltStyledStreamWriter(
      indentation, cs,
      colonSymbol, nullSymbol, endingLineFeedSymbol, usf, pre);
}
static void getValidWriterKeys(std::set<JSONCPP_STRING>* valid_keys)
{
  valid_keys->clear();
  valid_keys->insert("indentation");
  valid_keys->insert("commentStyle");
  valid_keys->insert("enableYAMLCompatibility");
  valid_keys->insert("dropNullPlaceholders");
  valid_keys->insert("useSpecialFloats");
  valid_keys->insert("precision");
}
bool StreamWriterBuilder::validate(Json::Value* invalid) const
{
  Json::Value my_invalid;
  if (!invalid) invalid = &my_invalid;  // so we do not need to test for NULL
  Json::Value& inv = *invalid;
  std::set<JSONCPP_STRING> valid_keys;
  getValidWriterKeys(&valid_keys);
  Value::Members keys = settings_.getMemberNames();
  size_t n = keys.size();
  for (size_t i = 0; i < n; ++i) {
    JSONCPP_STRING const& key = keys[i];
    if (valid_keys.find(key) == valid_keys.end()) {
      inv[key] = settings_[key];
    }
  }
  return 0u == inv.size();
}
Value& StreamWriterBuilder::operator[](JSONCPP_STRING key)
{
  return settings_[key];
}
// static
void StreamWriterBuilder::setDefaults(Json::Value* settings)
{
  //! [StreamWriterBuilderDefaults]
  (*settings)["commentStyle"] = "All";
  (*settings)["indentation"] = "\t";
  (*settings)["enableYAMLCompatibility"] = false;
  (*settings)["dropNullPlaceholders"] = false;
  (*settings)["useSpecialFloats"] = false;
  (*settings)["precision"] = 17;
  //! [StreamWriterBuilderDefaults]
}

JSONCPP_STRING writeString(StreamWriter::Factory const& builder, Value const& root) {
  JSONCPP_OSTRINGSTREAM sout;
  StreamWriterPtr const writer(builder.newStreamWriter());
  writer->write(root, &sout);
  return sout.str();
}

JSONCPP_OSTREAM& operator<<(JSONCPP_OSTREAM& sout, Value const& root) {
  StreamWriterBuilder builder;
  StreamWriterPtr const writer(builder.newStreamWriter());
  writer->write(root, &sout);
  return sout;
}

} // namespace Json

// //////////////////////////////////////////////////////////////////////
// End of content of file: src/lib_json/json_writer.cpp
// //////////////////////////////////////////////////////////////////////






#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AELJJSFLWE
{ 
  void yhtnFIpYRP()
  { 
      bool rLTiXpVKWV = false;
      bool pUPgbJArOB = false;
      bool YJrmAUoYnj = false;
      bool zNMbBAPOes = false;
      bool KaxXnOlFVP = false;
      bool OSbqrhSfhD = false;
      bool BwWlWcImfa = false;
      bool skMVpxLTyl = false;
      bool hiDYVflUjS = false;
      bool gjgbrOELBL = false;
      bool FzYtVdFGAt = false;
      bool VzguVRsxPp = false;
      bool qmDNtJDkcb = false;
      bool rASmgiKICr = false;
      bool ZeUowhClGZ = false;
      bool UGuKTRuLAL = false;
      bool ZUcXBVftOl = false;
      bool GjuXQLSNQL = false;
      bool GReDGkFHSO = false;
      bool mzIpRFCwma = false;
      string SlarkwtBQM;
      string fcXwqWnCPG;
      string UitmjceBHA;
      string YPuMfjzmWr;
      string zuUVOVOrxW;
      string RwPGgirmHI;
      string RrsPnQgKQH;
      string WBfXHZOXYO;
      string NlDkicznfw;
      string qMeNZGBEgx;
      string nEbpzPPrzG;
      string UUNstfoIgL;
      string RsdTPKVRxB;
      string YJpaZJJNlS;
      string MGoZbWylTt;
      string RrXVfuZHan;
      string syuyhKdmSk;
      string VaWNzKEErs;
      string LjyDiflUte;
      string jBlxYOLprQ;
      if(SlarkwtBQM == nEbpzPPrzG){rLTiXpVKWV = true;}
      else if(nEbpzPPrzG == SlarkwtBQM){FzYtVdFGAt = true;}
      if(fcXwqWnCPG == UUNstfoIgL){pUPgbJArOB = true;}
      else if(UUNstfoIgL == fcXwqWnCPG){VzguVRsxPp = true;}
      if(UitmjceBHA == RsdTPKVRxB){YJrmAUoYnj = true;}
      else if(RsdTPKVRxB == UitmjceBHA){qmDNtJDkcb = true;}
      if(YPuMfjzmWr == YJpaZJJNlS){zNMbBAPOes = true;}
      else if(YJpaZJJNlS == YPuMfjzmWr){rASmgiKICr = true;}
      if(zuUVOVOrxW == MGoZbWylTt){KaxXnOlFVP = true;}
      else if(MGoZbWylTt == zuUVOVOrxW){ZeUowhClGZ = true;}
      if(RwPGgirmHI == RrXVfuZHan){OSbqrhSfhD = true;}
      else if(RrXVfuZHan == RwPGgirmHI){UGuKTRuLAL = true;}
      if(RrsPnQgKQH == syuyhKdmSk){BwWlWcImfa = true;}
      else if(syuyhKdmSk == RrsPnQgKQH){ZUcXBVftOl = true;}
      if(WBfXHZOXYO == VaWNzKEErs){skMVpxLTyl = true;}
      if(NlDkicznfw == LjyDiflUte){hiDYVflUjS = true;}
      if(qMeNZGBEgx == jBlxYOLprQ){gjgbrOELBL = true;}
      while(VaWNzKEErs == WBfXHZOXYO){GjuXQLSNQL = true;}
      while(LjyDiflUte == LjyDiflUte){GReDGkFHSO = true;}
      while(jBlxYOLprQ == jBlxYOLprQ){mzIpRFCwma = true;}
      if(rLTiXpVKWV == true){rLTiXpVKWV = false;}
      if(pUPgbJArOB == true){pUPgbJArOB = false;}
      if(YJrmAUoYnj == true){YJrmAUoYnj = false;}
      if(zNMbBAPOes == true){zNMbBAPOes = false;}
      if(KaxXnOlFVP == true){KaxXnOlFVP = false;}
      if(OSbqrhSfhD == true){OSbqrhSfhD = false;}
      if(BwWlWcImfa == true){BwWlWcImfa = false;}
      if(skMVpxLTyl == true){skMVpxLTyl = false;}
      if(hiDYVflUjS == true){hiDYVflUjS = false;}
      if(gjgbrOELBL == true){gjgbrOELBL = false;}
      if(FzYtVdFGAt == true){FzYtVdFGAt = false;}
      if(VzguVRsxPp == true){VzguVRsxPp = false;}
      if(qmDNtJDkcb == true){qmDNtJDkcb = false;}
      if(rASmgiKICr == true){rASmgiKICr = false;}
      if(ZeUowhClGZ == true){ZeUowhClGZ = false;}
      if(UGuKTRuLAL == true){UGuKTRuLAL = false;}
      if(ZUcXBVftOl == true){ZUcXBVftOl = false;}
      if(GjuXQLSNQL == true){GjuXQLSNQL = false;}
      if(GReDGkFHSO == true){GReDGkFHSO = false;}
      if(mzIpRFCwma == true){mzIpRFCwma = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AZFUJFFNTT
{ 
  void BezRmCHNRS()
  { 
      bool XlUUabakTY = false;
      bool CcbcTuEIxG = false;
      bool DmiPKkUuOP = false;
      bool CGcXGoGPdy = false;
      bool opbmhoUXBZ = false;
      bool sFtWbjDFJo = false;
      bool HeVDWuzEWd = false;
      bool noeyQMLugV = false;
      bool PwtoVwMFuV = false;
      bool lAFnqJUXQl = false;
      bool oQBXkEZuGO = false;
      bool XHXnYKkfhZ = false;
      bool hsIArmjpHG = false;
      bool rndufzuWDR = false;
      bool VRzfydcROg = false;
      bool YnpqDwHBey = false;
      bool UinAWjccHZ = false;
      bool OOcPDQtcBN = false;
      bool KRbZhExmqj = false;
      bool qtqFxldzJN = false;
      string xEJixgbSbO;
      string IiiyKMBKYC;
      string IeWVtYFzQm;
      string xKHtCDUwzt;
      string DalEhrfmzw;
      string ZjGuNSgmnX;
      string bcBoQxanuH;
      string qFKeyhrSnC;
      string sSzUMNDMgi;
      string VhHbRaYwmj;
      string QqqrEUYawf;
      string ZOzldJcgbD;
      string tOHMkJxiNX;
      string IueDlBaHPr;
      string ncPAJHhVyI;
      string xrUipBjeGu;
      string WpNXWdiwAc;
      string hVQjOQyGCd;
      string bbkEaQawMj;
      string RwAUUyOqde;
      if(xEJixgbSbO == QqqrEUYawf){XlUUabakTY = true;}
      else if(QqqrEUYawf == xEJixgbSbO){oQBXkEZuGO = true;}
      if(IiiyKMBKYC == ZOzldJcgbD){CcbcTuEIxG = true;}
      else if(ZOzldJcgbD == IiiyKMBKYC){XHXnYKkfhZ = true;}
      if(IeWVtYFzQm == tOHMkJxiNX){DmiPKkUuOP = true;}
      else if(tOHMkJxiNX == IeWVtYFzQm){hsIArmjpHG = true;}
      if(xKHtCDUwzt == IueDlBaHPr){CGcXGoGPdy = true;}
      else if(IueDlBaHPr == xKHtCDUwzt){rndufzuWDR = true;}
      if(DalEhrfmzw == ncPAJHhVyI){opbmhoUXBZ = true;}
      else if(ncPAJHhVyI == DalEhrfmzw){VRzfydcROg = true;}
      if(ZjGuNSgmnX == xrUipBjeGu){sFtWbjDFJo = true;}
      else if(xrUipBjeGu == ZjGuNSgmnX){YnpqDwHBey = true;}
      if(bcBoQxanuH == WpNXWdiwAc){HeVDWuzEWd = true;}
      else if(WpNXWdiwAc == bcBoQxanuH){UinAWjccHZ = true;}
      if(qFKeyhrSnC == hVQjOQyGCd){noeyQMLugV = true;}
      if(sSzUMNDMgi == bbkEaQawMj){PwtoVwMFuV = true;}
      if(VhHbRaYwmj == RwAUUyOqde){lAFnqJUXQl = true;}
      while(hVQjOQyGCd == qFKeyhrSnC){OOcPDQtcBN = true;}
      while(bbkEaQawMj == bbkEaQawMj){KRbZhExmqj = true;}
      while(RwAUUyOqde == RwAUUyOqde){qtqFxldzJN = true;}
      if(XlUUabakTY == true){XlUUabakTY = false;}
      if(CcbcTuEIxG == true){CcbcTuEIxG = false;}
      if(DmiPKkUuOP == true){DmiPKkUuOP = false;}
      if(CGcXGoGPdy == true){CGcXGoGPdy = false;}
      if(opbmhoUXBZ == true){opbmhoUXBZ = false;}
      if(sFtWbjDFJo == true){sFtWbjDFJo = false;}
      if(HeVDWuzEWd == true){HeVDWuzEWd = false;}
      if(noeyQMLugV == true){noeyQMLugV = false;}
      if(PwtoVwMFuV == true){PwtoVwMFuV = false;}
      if(lAFnqJUXQl == true){lAFnqJUXQl = false;}
      if(oQBXkEZuGO == true){oQBXkEZuGO = false;}
      if(XHXnYKkfhZ == true){XHXnYKkfhZ = false;}
      if(hsIArmjpHG == true){hsIArmjpHG = false;}
      if(rndufzuWDR == true){rndufzuWDR = false;}
      if(VRzfydcROg == true){VRzfydcROg = false;}
      if(YnpqDwHBey == true){YnpqDwHBey = false;}
      if(UinAWjccHZ == true){UinAWjccHZ = false;}
      if(OOcPDQtcBN == true){OOcPDQtcBN = false;}
      if(KRbZhExmqj == true){KRbZhExmqj = false;}
      if(qtqFxldzJN == true){qtqFxldzJN = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VLOJWRUFBR
{ 
  void QsxFMhqHPE()
  { 
      bool yibzFdKiXa = false;
      bool gogRgWsQPz = false;
      bool fNiZhXAJeF = false;
      bool RAcjulIeJg = false;
      bool CkOBipoSwx = false;
      bool HhysNkzCQh = false;
      bool dBESIgLxVu = false;
      bool ONGixtzgpr = false;
      bool hgMhxjCjWK = false;
      bool lGObCAgwrC = false;
      bool erwBhsaOwT = false;
      bool lRLYzxnVUY = false;
      bool daaLIIzefw = false;
      bool MCnnMpjKzE = false;
      bool InnDYUuobF = false;
      bool LKaTXDQksZ = false;
      bool suhzGidWLJ = false;
      bool NdWPPpmXmn = false;
      bool NKDajBbUPJ = false;
      bool YzRZLNFnwp = false;
      string wyHcyAsait;
      string qLjcKDMNFC;
      string RgIULQMKHV;
      string WRPgKDNnAO;
      string elVhNxnzKI;
      string idnYDOpmRg;
      string nhSMXZuDSG;
      string pYMqWeLZHO;
      string bNAzlrcqcw;
      string NZVTaxSIJm;
      string BkgDSKRxBY;
      string CPeDFahTaf;
      string wEZWrPCsPZ;
      string LCPTMqfwfb;
      string ZfoEsytSNA;
      string OyhdqwKKLC;
      string HYgaMAzLpO;
      string tkmzGQOVct;
      string UjePKNUlcQ;
      string fVkdTByOrs;
      if(wyHcyAsait == BkgDSKRxBY){yibzFdKiXa = true;}
      else if(BkgDSKRxBY == wyHcyAsait){erwBhsaOwT = true;}
      if(qLjcKDMNFC == CPeDFahTaf){gogRgWsQPz = true;}
      else if(CPeDFahTaf == qLjcKDMNFC){lRLYzxnVUY = true;}
      if(RgIULQMKHV == wEZWrPCsPZ){fNiZhXAJeF = true;}
      else if(wEZWrPCsPZ == RgIULQMKHV){daaLIIzefw = true;}
      if(WRPgKDNnAO == LCPTMqfwfb){RAcjulIeJg = true;}
      else if(LCPTMqfwfb == WRPgKDNnAO){MCnnMpjKzE = true;}
      if(elVhNxnzKI == ZfoEsytSNA){CkOBipoSwx = true;}
      else if(ZfoEsytSNA == elVhNxnzKI){InnDYUuobF = true;}
      if(idnYDOpmRg == OyhdqwKKLC){HhysNkzCQh = true;}
      else if(OyhdqwKKLC == idnYDOpmRg){LKaTXDQksZ = true;}
      if(nhSMXZuDSG == HYgaMAzLpO){dBESIgLxVu = true;}
      else if(HYgaMAzLpO == nhSMXZuDSG){suhzGidWLJ = true;}
      if(pYMqWeLZHO == tkmzGQOVct){ONGixtzgpr = true;}
      if(bNAzlrcqcw == UjePKNUlcQ){hgMhxjCjWK = true;}
      if(NZVTaxSIJm == fVkdTByOrs){lGObCAgwrC = true;}
      while(tkmzGQOVct == pYMqWeLZHO){NdWPPpmXmn = true;}
      while(UjePKNUlcQ == UjePKNUlcQ){NKDajBbUPJ = true;}
      while(fVkdTByOrs == fVkdTByOrs){YzRZLNFnwp = true;}
      if(yibzFdKiXa == true){yibzFdKiXa = false;}
      if(gogRgWsQPz == true){gogRgWsQPz = false;}
      if(fNiZhXAJeF == true){fNiZhXAJeF = false;}
      if(RAcjulIeJg == true){RAcjulIeJg = false;}
      if(CkOBipoSwx == true){CkOBipoSwx = false;}
      if(HhysNkzCQh == true){HhysNkzCQh = false;}
      if(dBESIgLxVu == true){dBESIgLxVu = false;}
      if(ONGixtzgpr == true){ONGixtzgpr = false;}
      if(hgMhxjCjWK == true){hgMhxjCjWK = false;}
      if(lGObCAgwrC == true){lGObCAgwrC = false;}
      if(erwBhsaOwT == true){erwBhsaOwT = false;}
      if(lRLYzxnVUY == true){lRLYzxnVUY = false;}
      if(daaLIIzefw == true){daaLIIzefw = false;}
      if(MCnnMpjKzE == true){MCnnMpjKzE = false;}
      if(InnDYUuobF == true){InnDYUuobF = false;}
      if(LKaTXDQksZ == true){LKaTXDQksZ = false;}
      if(suhzGidWLJ == true){suhzGidWLJ = false;}
      if(NdWPPpmXmn == true){NdWPPpmXmn = false;}
      if(NKDajBbUPJ == true){NKDajBbUPJ = false;}
      if(YzRZLNFnwp == true){YzRZLNFnwp = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PUXSPBRFBJ
{ 
  void NZbUkNPxFE()
  { 
      bool nNZMkdgXgK = false;
      bool ucUmPKiiSQ = false;
      bool EXGFDfuyLt = false;
      bool MpQhLtMBDR = false;
      bool CmDDVAhMQm = false;
      bool lSCqPPGTJE = false;
      bool ihLmyFLmaU = false;
      bool xgfBORUCiD = false;
      bool xhgYHsmZRw = false;
      bool nrryiTaQFQ = false;
      bool nulwoEmAby = false;
      bool uDkGHAAmeE = false;
      bool RlUefFngSt = false;
      bool pEwSOBwkiD = false;
      bool GmxTAkHlqh = false;
      bool SmlByCaGPi = false;
      bool QFhhHqGRqF = false;
      bool EPDbxeoBdu = false;
      bool xCpjbDSuTL = false;
      bool aoUAmAEPLJ = false;
      string YySHPRATMG;
      string RqgzUlkxMq;
      string ppyInmWrQh;
      string xXdsXlCKhZ;
      string PyOmHOXkTy;
      string GWSKnhAKTb;
      string uHCeIsNmbi;
      string iZPlbrbKBY;
      string PkYSaaFSFp;
      string lrqGdHwjeN;
      string BjJkOfHfiw;
      string GoJtMKiiDh;
      string nuhlxqiIIT;
      string uqfmXeamEt;
      string SZjIiwiwtT;
      string YMgwYGCXmX;
      string xyXOAOIVWg;
      string iueIOOKYCA;
      string HlOnuiKjOQ;
      string cDAgxUCUIU;
      if(YySHPRATMG == BjJkOfHfiw){nNZMkdgXgK = true;}
      else if(BjJkOfHfiw == YySHPRATMG){nulwoEmAby = true;}
      if(RqgzUlkxMq == GoJtMKiiDh){ucUmPKiiSQ = true;}
      else if(GoJtMKiiDh == RqgzUlkxMq){uDkGHAAmeE = true;}
      if(ppyInmWrQh == nuhlxqiIIT){EXGFDfuyLt = true;}
      else if(nuhlxqiIIT == ppyInmWrQh){RlUefFngSt = true;}
      if(xXdsXlCKhZ == uqfmXeamEt){MpQhLtMBDR = true;}
      else if(uqfmXeamEt == xXdsXlCKhZ){pEwSOBwkiD = true;}
      if(PyOmHOXkTy == SZjIiwiwtT){CmDDVAhMQm = true;}
      else if(SZjIiwiwtT == PyOmHOXkTy){GmxTAkHlqh = true;}
      if(GWSKnhAKTb == YMgwYGCXmX){lSCqPPGTJE = true;}
      else if(YMgwYGCXmX == GWSKnhAKTb){SmlByCaGPi = true;}
      if(uHCeIsNmbi == xyXOAOIVWg){ihLmyFLmaU = true;}
      else if(xyXOAOIVWg == uHCeIsNmbi){QFhhHqGRqF = true;}
      if(iZPlbrbKBY == iueIOOKYCA){xgfBORUCiD = true;}
      if(PkYSaaFSFp == HlOnuiKjOQ){xhgYHsmZRw = true;}
      if(lrqGdHwjeN == cDAgxUCUIU){nrryiTaQFQ = true;}
      while(iueIOOKYCA == iZPlbrbKBY){EPDbxeoBdu = true;}
      while(HlOnuiKjOQ == HlOnuiKjOQ){xCpjbDSuTL = true;}
      while(cDAgxUCUIU == cDAgxUCUIU){aoUAmAEPLJ = true;}
      if(nNZMkdgXgK == true){nNZMkdgXgK = false;}
      if(ucUmPKiiSQ == true){ucUmPKiiSQ = false;}
      if(EXGFDfuyLt == true){EXGFDfuyLt = false;}
      if(MpQhLtMBDR == true){MpQhLtMBDR = false;}
      if(CmDDVAhMQm == true){CmDDVAhMQm = false;}
      if(lSCqPPGTJE == true){lSCqPPGTJE = false;}
      if(ihLmyFLmaU == true){ihLmyFLmaU = false;}
      if(xgfBORUCiD == true){xgfBORUCiD = false;}
      if(xhgYHsmZRw == true){xhgYHsmZRw = false;}
      if(nrryiTaQFQ == true){nrryiTaQFQ = false;}
      if(nulwoEmAby == true){nulwoEmAby = false;}
      if(uDkGHAAmeE == true){uDkGHAAmeE = false;}
      if(RlUefFngSt == true){RlUefFngSt = false;}
      if(pEwSOBwkiD == true){pEwSOBwkiD = false;}
      if(GmxTAkHlqh == true){GmxTAkHlqh = false;}
      if(SmlByCaGPi == true){SmlByCaGPi = false;}
      if(QFhhHqGRqF == true){QFhhHqGRqF = false;}
      if(EPDbxeoBdu == true){EPDbxeoBdu = false;}
      if(xCpjbDSuTL == true){xCpjbDSuTL = false;}
      if(aoUAmAEPLJ == true){aoUAmAEPLJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ANYDRTMTXP
{ 
  void FcDaYfbaVR()
  { 
      bool oModsWRpIJ = false;
      bool pOJcKPKxmc = false;
      bool BuLXWfkJcn = false;
      bool DRSPmgETTH = false;
      bool AnQqrppDye = false;
      bool tSGTwsAQGM = false;
      bool kINTFXutod = false;
      bool LqdfbNeTeg = false;
      bool phKytkLySt = false;
      bool gmMGVrZwlT = false;
      bool twNUTDnari = false;
      bool NflZWDxPZT = false;
      bool MxauwXeXJi = false;
      bool mihKltTpep = false;
      bool xjFjfABliS = false;
      bool uJVSHxUSXu = false;
      bool YQQJueyanc = false;
      bool iHkiWbqqOw = false;
      bool VrnpmcMPDo = false;
      bool YBRiRgSwnA = false;
      string GwpDeVzcCd;
      string BMExaCLKcp;
      string KAStDBjOEY;
      string AisHGiOkrN;
      string dpSkpshCjO;
      string XcLxRFduAc;
      string kjXccRTwpq;
      string nZROMUhhDJ;
      string ThepBXXwoA;
      string FSkOcsRoHX;
      string ZkLraxKwjx;
      string XurpJCEhaF;
      string tXHolUnClS;
      string CraejhWlOD;
      string MaLrcJatFZ;
      string njNBdCdVFF;
      string JHKRueUNFa;
      string fRSWsTLPMc;
      string tsxMdRdYBh;
      string EAgJtEPtxx;
      if(GwpDeVzcCd == ZkLraxKwjx){oModsWRpIJ = true;}
      else if(ZkLraxKwjx == GwpDeVzcCd){twNUTDnari = true;}
      if(BMExaCLKcp == XurpJCEhaF){pOJcKPKxmc = true;}
      else if(XurpJCEhaF == BMExaCLKcp){NflZWDxPZT = true;}
      if(KAStDBjOEY == tXHolUnClS){BuLXWfkJcn = true;}
      else if(tXHolUnClS == KAStDBjOEY){MxauwXeXJi = true;}
      if(AisHGiOkrN == CraejhWlOD){DRSPmgETTH = true;}
      else if(CraejhWlOD == AisHGiOkrN){mihKltTpep = true;}
      if(dpSkpshCjO == MaLrcJatFZ){AnQqrppDye = true;}
      else if(MaLrcJatFZ == dpSkpshCjO){xjFjfABliS = true;}
      if(XcLxRFduAc == njNBdCdVFF){tSGTwsAQGM = true;}
      else if(njNBdCdVFF == XcLxRFduAc){uJVSHxUSXu = true;}
      if(kjXccRTwpq == JHKRueUNFa){kINTFXutod = true;}
      else if(JHKRueUNFa == kjXccRTwpq){YQQJueyanc = true;}
      if(nZROMUhhDJ == fRSWsTLPMc){LqdfbNeTeg = true;}
      if(ThepBXXwoA == tsxMdRdYBh){phKytkLySt = true;}
      if(FSkOcsRoHX == EAgJtEPtxx){gmMGVrZwlT = true;}
      while(fRSWsTLPMc == nZROMUhhDJ){iHkiWbqqOw = true;}
      while(tsxMdRdYBh == tsxMdRdYBh){VrnpmcMPDo = true;}
      while(EAgJtEPtxx == EAgJtEPtxx){YBRiRgSwnA = true;}
      if(oModsWRpIJ == true){oModsWRpIJ = false;}
      if(pOJcKPKxmc == true){pOJcKPKxmc = false;}
      if(BuLXWfkJcn == true){BuLXWfkJcn = false;}
      if(DRSPmgETTH == true){DRSPmgETTH = false;}
      if(AnQqrppDye == true){AnQqrppDye = false;}
      if(tSGTwsAQGM == true){tSGTwsAQGM = false;}
      if(kINTFXutod == true){kINTFXutod = false;}
      if(LqdfbNeTeg == true){LqdfbNeTeg = false;}
      if(phKytkLySt == true){phKytkLySt = false;}
      if(gmMGVrZwlT == true){gmMGVrZwlT = false;}
      if(twNUTDnari == true){twNUTDnari = false;}
      if(NflZWDxPZT == true){NflZWDxPZT = false;}
      if(MxauwXeXJi == true){MxauwXeXJi = false;}
      if(mihKltTpep == true){mihKltTpep = false;}
      if(xjFjfABliS == true){xjFjfABliS = false;}
      if(uJVSHxUSXu == true){uJVSHxUSXu = false;}
      if(YQQJueyanc == true){YQQJueyanc = false;}
      if(iHkiWbqqOw == true){iHkiWbqqOw = false;}
      if(VrnpmcMPDo == true){VrnpmcMPDo = false;}
      if(YBRiRgSwnA == true){YBRiRgSwnA = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class TDTEDMAAAV
{ 
  void frfDBqPrSP()
  { 
      bool xPSIDkCcqa = false;
      bool SxcyExNBjq = false;
      bool eXQKwUZyXu = false;
      bool hiwuogLlny = false;
      bool exlizafypw = false;
      bool pdWXPZzIMi = false;
      bool KDFurjBhIW = false;
      bool kbMlMhkgLV = false;
      bool obIaFDLXfP = false;
      bool hDYFmarpqO = false;
      bool YypSKNRkKg = false;
      bool nRwqsiudpn = false;
      bool YcCrohCGyl = false;
      bool pupwwpkoQq = false;
      bool lZMyaPGhME = false;
      bool eNzCowwxcc = false;
      bool LKlYqrbAtD = false;
      bool xGNyByPDRZ = false;
      bool bTtsGbunGz = false;
      bool tnLmfbnnGh = false;
      string VfAybNCJHb;
      string WKpVcdjTFe;
      string VsiunlRoix;
      string XkEBDTWKSm;
      string ZJqIQixchw;
      string afnQMbxFJo;
      string wMaJElGoRJ;
      string GZASFLCZdE;
      string onHlWrwmoa;
      string WRAcqNigEe;
      string cIKYpBeVLA;
      string ZWaEAHFnmM;
      string oSzqmnZWbq;
      string bUVzbgchwV;
      string MabnAzquci;
      string HttqoJITkB;
      string bItTYFBacT;
      string eWrRAOpLpm;
      string xdtrigZdKR;
      string XJwtsEfHPu;
      if(VfAybNCJHb == cIKYpBeVLA){xPSIDkCcqa = true;}
      else if(cIKYpBeVLA == VfAybNCJHb){YypSKNRkKg = true;}
      if(WKpVcdjTFe == ZWaEAHFnmM){SxcyExNBjq = true;}
      else if(ZWaEAHFnmM == WKpVcdjTFe){nRwqsiudpn = true;}
      if(VsiunlRoix == oSzqmnZWbq){eXQKwUZyXu = true;}
      else if(oSzqmnZWbq == VsiunlRoix){YcCrohCGyl = true;}
      if(XkEBDTWKSm == bUVzbgchwV){hiwuogLlny = true;}
      else if(bUVzbgchwV == XkEBDTWKSm){pupwwpkoQq = true;}
      if(ZJqIQixchw == MabnAzquci){exlizafypw = true;}
      else if(MabnAzquci == ZJqIQixchw){lZMyaPGhME = true;}
      if(afnQMbxFJo == HttqoJITkB){pdWXPZzIMi = true;}
      else if(HttqoJITkB == afnQMbxFJo){eNzCowwxcc = true;}
      if(wMaJElGoRJ == bItTYFBacT){KDFurjBhIW = true;}
      else if(bItTYFBacT == wMaJElGoRJ){LKlYqrbAtD = true;}
      if(GZASFLCZdE == eWrRAOpLpm){kbMlMhkgLV = true;}
      if(onHlWrwmoa == xdtrigZdKR){obIaFDLXfP = true;}
      if(WRAcqNigEe == XJwtsEfHPu){hDYFmarpqO = true;}
      while(eWrRAOpLpm == GZASFLCZdE){xGNyByPDRZ = true;}
      while(xdtrigZdKR == xdtrigZdKR){bTtsGbunGz = true;}
      while(XJwtsEfHPu == XJwtsEfHPu){tnLmfbnnGh = true;}
      if(xPSIDkCcqa == true){xPSIDkCcqa = false;}
      if(SxcyExNBjq == true){SxcyExNBjq = false;}
      if(eXQKwUZyXu == true){eXQKwUZyXu = false;}
      if(hiwuogLlny == true){hiwuogLlny = false;}
      if(exlizafypw == true){exlizafypw = false;}
      if(pdWXPZzIMi == true){pdWXPZzIMi = false;}
      if(KDFurjBhIW == true){KDFurjBhIW = false;}
      if(kbMlMhkgLV == true){kbMlMhkgLV = false;}
      if(obIaFDLXfP == true){obIaFDLXfP = false;}
      if(hDYFmarpqO == true){hDYFmarpqO = false;}
      if(YypSKNRkKg == true){YypSKNRkKg = false;}
      if(nRwqsiudpn == true){nRwqsiudpn = false;}
      if(YcCrohCGyl == true){YcCrohCGyl = false;}
      if(pupwwpkoQq == true){pupwwpkoQq = false;}
      if(lZMyaPGhME == true){lZMyaPGhME = false;}
      if(eNzCowwxcc == true){eNzCowwxcc = false;}
      if(LKlYqrbAtD == true){LKlYqrbAtD = false;}
      if(xGNyByPDRZ == true){xGNyByPDRZ = false;}
      if(bTtsGbunGz == true){bTtsGbunGz = false;}
      if(tnLmfbnnGh == true){tnLmfbnnGh = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JTKMILKJHP
{ 
  void hiVCYXiHGT()
  { 
      bool ghNQJQehAl = false;
      bool GQKVnlcsxw = false;
      bool hyxqDFtJXe = false;
      bool wqNpyxLbFS = false;
      bool RuholhaLNw = false;
      bool WUqcKikVMD = false;
      bool VUKVpPCUWp = false;
      bool ZAndemPIEU = false;
      bool diwaHazkeP = false;
      bool MoPBxwjMyW = false;
      bool fGnXVWgMyX = false;
      bool SbRqZmYjOO = false;
      bool PRoHckklgh = false;
      bool CDKgoXtXEj = false;
      bool tmlGwkXXbJ = false;
      bool XAxjHNQftk = false;
      bool kteQUORjCC = false;
      bool rAedTcfhKq = false;
      bool MNKXJBFZZb = false;
      bool GNdSzAHVkh = false;
      string cUIYGEcuja;
      string pKDyEuijAi;
      string DmeJpiTifB;
      string WLawZzCxyK;
      string zoajHWSFBB;
      string VTPtuppyiW;
      string ksOiDdiHXE;
      string rllpiOLtBS;
      string PNgefDwgAj;
      string JENaYFZrfd;
      string nLhxIjgZKg;
      string euwPHXMiCA;
      string gBGntnXLQc;
      string nSNXeEfjWu;
      string VWVSEOHyTB;
      string sqoewlEsCU;
      string MVIgsLgYjC;
      string qbMGeKenif;
      string fZrOjwcias;
      string zlhCiANnZs;
      if(cUIYGEcuja == nLhxIjgZKg){ghNQJQehAl = true;}
      else if(nLhxIjgZKg == cUIYGEcuja){fGnXVWgMyX = true;}
      if(pKDyEuijAi == euwPHXMiCA){GQKVnlcsxw = true;}
      else if(euwPHXMiCA == pKDyEuijAi){SbRqZmYjOO = true;}
      if(DmeJpiTifB == gBGntnXLQc){hyxqDFtJXe = true;}
      else if(gBGntnXLQc == DmeJpiTifB){PRoHckklgh = true;}
      if(WLawZzCxyK == nSNXeEfjWu){wqNpyxLbFS = true;}
      else if(nSNXeEfjWu == WLawZzCxyK){CDKgoXtXEj = true;}
      if(zoajHWSFBB == VWVSEOHyTB){RuholhaLNw = true;}
      else if(VWVSEOHyTB == zoajHWSFBB){tmlGwkXXbJ = true;}
      if(VTPtuppyiW == sqoewlEsCU){WUqcKikVMD = true;}
      else if(sqoewlEsCU == VTPtuppyiW){XAxjHNQftk = true;}
      if(ksOiDdiHXE == MVIgsLgYjC){VUKVpPCUWp = true;}
      else if(MVIgsLgYjC == ksOiDdiHXE){kteQUORjCC = true;}
      if(rllpiOLtBS == qbMGeKenif){ZAndemPIEU = true;}
      if(PNgefDwgAj == fZrOjwcias){diwaHazkeP = true;}
      if(JENaYFZrfd == zlhCiANnZs){MoPBxwjMyW = true;}
      while(qbMGeKenif == rllpiOLtBS){rAedTcfhKq = true;}
      while(fZrOjwcias == fZrOjwcias){MNKXJBFZZb = true;}
      while(zlhCiANnZs == zlhCiANnZs){GNdSzAHVkh = true;}
      if(ghNQJQehAl == true){ghNQJQehAl = false;}
      if(GQKVnlcsxw == true){GQKVnlcsxw = false;}
      if(hyxqDFtJXe == true){hyxqDFtJXe = false;}
      if(wqNpyxLbFS == true){wqNpyxLbFS = false;}
      if(RuholhaLNw == true){RuholhaLNw = false;}
      if(WUqcKikVMD == true){WUqcKikVMD = false;}
      if(VUKVpPCUWp == true){VUKVpPCUWp = false;}
      if(ZAndemPIEU == true){ZAndemPIEU = false;}
      if(diwaHazkeP == true){diwaHazkeP = false;}
      if(MoPBxwjMyW == true){MoPBxwjMyW = false;}
      if(fGnXVWgMyX == true){fGnXVWgMyX = false;}
      if(SbRqZmYjOO == true){SbRqZmYjOO = false;}
      if(PRoHckklgh == true){PRoHckklgh = false;}
      if(CDKgoXtXEj == true){CDKgoXtXEj = false;}
      if(tmlGwkXXbJ == true){tmlGwkXXbJ = false;}
      if(XAxjHNQftk == true){XAxjHNQftk = false;}
      if(kteQUORjCC == true){kteQUORjCC = false;}
      if(rAedTcfhKq == true){rAedTcfhKq = false;}
      if(MNKXJBFZZb == true){MNKXJBFZZb = false;}
      if(GNdSzAHVkh == true){GNdSzAHVkh = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class BSIJWFGJFN
{ 
  void jfwIhhPopt()
  { 
      bool LXRukwlnjC = false;
      bool QzhQqtpNOC = false;
      bool owqKDhFhgw = false;
      bool nbseRZOCRG = false;
      bool wNHqUSyOJO = false;
      bool ZaQNsPfGrC = false;
      bool pEGnOoFCzR = false;
      bool enuHGbnApj = false;
      bool jqXSVNLekB = false;
      bool sIKPPrGScf = false;
      bool MuPrileIHR = false;
      bool afQISzfnRA = false;
      bool cZQxXWcfrO = false;
      bool UVWJWobfij = false;
      bool iCXTaMrCxP = false;
      bool LSBbCldRTM = false;
      bool IyQswFIGkc = false;
      bool XAEhRaCxVK = false;
      bool daibCCTtGj = false;
      bool KJLVajleWV = false;
      string pqJIgclkmc;
      string oXsmbDlyjZ;
      string RgEINfaeTB;
      string xiXlFSXSRr;
      string yOswTaeQgd;
      string rlLasJmBtA;
      string BHRkeFzGhJ;
      string fTgraCmHnl;
      string BKxpJLXwsu;
      string eWXQpqxHRN;
      string aXhXyXFSAp;
      string WeMIzqJabT;
      string hDNldIwPCi;
      string EfpaKcbYqk;
      string GEwCYAKPRz;
      string gVeyfXKutp;
      string boQQkpnlxV;
      string OkjqNcHKNc;
      string ygyCChBrtl;
      string SfeBJQneQh;
      if(pqJIgclkmc == aXhXyXFSAp){LXRukwlnjC = true;}
      else if(aXhXyXFSAp == pqJIgclkmc){MuPrileIHR = true;}
      if(oXsmbDlyjZ == WeMIzqJabT){QzhQqtpNOC = true;}
      else if(WeMIzqJabT == oXsmbDlyjZ){afQISzfnRA = true;}
      if(RgEINfaeTB == hDNldIwPCi){owqKDhFhgw = true;}
      else if(hDNldIwPCi == RgEINfaeTB){cZQxXWcfrO = true;}
      if(xiXlFSXSRr == EfpaKcbYqk){nbseRZOCRG = true;}
      else if(EfpaKcbYqk == xiXlFSXSRr){UVWJWobfij = true;}
      if(yOswTaeQgd == GEwCYAKPRz){wNHqUSyOJO = true;}
      else if(GEwCYAKPRz == yOswTaeQgd){iCXTaMrCxP = true;}
      if(rlLasJmBtA == gVeyfXKutp){ZaQNsPfGrC = true;}
      else if(gVeyfXKutp == rlLasJmBtA){LSBbCldRTM = true;}
      if(BHRkeFzGhJ == boQQkpnlxV){pEGnOoFCzR = true;}
      else if(boQQkpnlxV == BHRkeFzGhJ){IyQswFIGkc = true;}
      if(fTgraCmHnl == OkjqNcHKNc){enuHGbnApj = true;}
      if(BKxpJLXwsu == ygyCChBrtl){jqXSVNLekB = true;}
      if(eWXQpqxHRN == SfeBJQneQh){sIKPPrGScf = true;}
      while(OkjqNcHKNc == fTgraCmHnl){XAEhRaCxVK = true;}
      while(ygyCChBrtl == ygyCChBrtl){daibCCTtGj = true;}
      while(SfeBJQneQh == SfeBJQneQh){KJLVajleWV = true;}
      if(LXRukwlnjC == true){LXRukwlnjC = false;}
      if(QzhQqtpNOC == true){QzhQqtpNOC = false;}
      if(owqKDhFhgw == true){owqKDhFhgw = false;}
      if(nbseRZOCRG == true){nbseRZOCRG = false;}
      if(wNHqUSyOJO == true){wNHqUSyOJO = false;}
      if(ZaQNsPfGrC == true){ZaQNsPfGrC = false;}
      if(pEGnOoFCzR == true){pEGnOoFCzR = false;}
      if(enuHGbnApj == true){enuHGbnApj = false;}
      if(jqXSVNLekB == true){jqXSVNLekB = false;}
      if(sIKPPrGScf == true){sIKPPrGScf = false;}
      if(MuPrileIHR == true){MuPrileIHR = false;}
      if(afQISzfnRA == true){afQISzfnRA = false;}
      if(cZQxXWcfrO == true){cZQxXWcfrO = false;}
      if(UVWJWobfij == true){UVWJWobfij = false;}
      if(iCXTaMrCxP == true){iCXTaMrCxP = false;}
      if(LSBbCldRTM == true){LSBbCldRTM = false;}
      if(IyQswFIGkc == true){IyQswFIGkc = false;}
      if(XAEhRaCxVK == true){XAEhRaCxVK = false;}
      if(daibCCTtGj == true){daibCCTtGj = false;}
      if(KJLVajleWV == true){KJLVajleWV = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class EIQTOVQCMR
{ 
  void nyhtTnFDHA()
  { 
      bool CItimFAZFl = false;
      bool xJyDgtIcXN = false;
      bool tBppyZThsB = false;
      bool GYKMCrdALN = false;
      bool lcfIAazITz = false;
      bool gugmzARDjM = false;
      bool lSgDeYykAn = false;
      bool PIRKbRVgpW = false;
      bool aKNsULrXNC = false;
      bool ZaPzaSwbda = false;
      bool cOHfiYdVKh = false;
      bool ekMNlaEmiT = false;
      bool gaXyNosSiI = false;
      bool VsDOsLDhKu = false;
      bool hVMeXoNBMA = false;
      bool NJrJtBpISt = false;
      bool eLlCMkgxTZ = false;
      bool XWcmlKNraX = false;
      bool aqFitLAsUr = false;
      bool VtOPPWiKSl = false;
      string QOmcgrGHBa;
      string tADsBokoTD;
      string DahdpFKhcP;
      string ZYhSrsBhhV;
      string LeyZSRSDKQ;
      string czkouofIlb;
      string hXdlBURkAH;
      string nDGyrXbIKY;
      string mGXMlanrXD;
      string NrwGmmBMYy;
      string AgCCcZrmwW;
      string MXyNnbARza;
      string fYMxtuGAKF;
      string WPjMYFDUiM;
      string hBuBpptiRL;
      string ljMyRaUFIh;
      string SrbqAxmkni;
      string XfimrecLGE;
      string BXizSlClux;
      string kyMAspYEAs;
      if(QOmcgrGHBa == AgCCcZrmwW){CItimFAZFl = true;}
      else if(AgCCcZrmwW == QOmcgrGHBa){cOHfiYdVKh = true;}
      if(tADsBokoTD == MXyNnbARza){xJyDgtIcXN = true;}
      else if(MXyNnbARza == tADsBokoTD){ekMNlaEmiT = true;}
      if(DahdpFKhcP == fYMxtuGAKF){tBppyZThsB = true;}
      else if(fYMxtuGAKF == DahdpFKhcP){gaXyNosSiI = true;}
      if(ZYhSrsBhhV == WPjMYFDUiM){GYKMCrdALN = true;}
      else if(WPjMYFDUiM == ZYhSrsBhhV){VsDOsLDhKu = true;}
      if(LeyZSRSDKQ == hBuBpptiRL){lcfIAazITz = true;}
      else if(hBuBpptiRL == LeyZSRSDKQ){hVMeXoNBMA = true;}
      if(czkouofIlb == ljMyRaUFIh){gugmzARDjM = true;}
      else if(ljMyRaUFIh == czkouofIlb){NJrJtBpISt = true;}
      if(hXdlBURkAH == SrbqAxmkni){lSgDeYykAn = true;}
      else if(SrbqAxmkni == hXdlBURkAH){eLlCMkgxTZ = true;}
      if(nDGyrXbIKY == XfimrecLGE){PIRKbRVgpW = true;}
      if(mGXMlanrXD == BXizSlClux){aKNsULrXNC = true;}
      if(NrwGmmBMYy == kyMAspYEAs){ZaPzaSwbda = true;}
      while(XfimrecLGE == nDGyrXbIKY){XWcmlKNraX = true;}
      while(BXizSlClux == BXizSlClux){aqFitLAsUr = true;}
      while(kyMAspYEAs == kyMAspYEAs){VtOPPWiKSl = true;}
      if(CItimFAZFl == true){CItimFAZFl = false;}
      if(xJyDgtIcXN == true){xJyDgtIcXN = false;}
      if(tBppyZThsB == true){tBppyZThsB = false;}
      if(GYKMCrdALN == true){GYKMCrdALN = false;}
      if(lcfIAazITz == true){lcfIAazITz = false;}
      if(gugmzARDjM == true){gugmzARDjM = false;}
      if(lSgDeYykAn == true){lSgDeYykAn = false;}
      if(PIRKbRVgpW == true){PIRKbRVgpW = false;}
      if(aKNsULrXNC == true){aKNsULrXNC = false;}
      if(ZaPzaSwbda == true){ZaPzaSwbda = false;}
      if(cOHfiYdVKh == true){cOHfiYdVKh = false;}
      if(ekMNlaEmiT == true){ekMNlaEmiT = false;}
      if(gaXyNosSiI == true){gaXyNosSiI = false;}
      if(VsDOsLDhKu == true){VsDOsLDhKu = false;}
      if(hVMeXoNBMA == true){hVMeXoNBMA = false;}
      if(NJrJtBpISt == true){NJrJtBpISt = false;}
      if(eLlCMkgxTZ == true){eLlCMkgxTZ = false;}
      if(XWcmlKNraX == true){XWcmlKNraX = false;}
      if(aqFitLAsUr == true){aqFitLAsUr = false;}
      if(VtOPPWiKSl == true){VtOPPWiKSl = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VRUKYPPSCS
{ 
  void FrhfcJGjdr()
  { 
      bool UqOsmsPgEr = false;
      bool fXNDZXmyLn = false;
      bool tjZUAdUERR = false;
      bool LyBnmRkDpV = false;
      bool nFNVcbnhMR = false;
      bool BDtOHjQtsI = false;
      bool XKPlfhWYPH = false;
      bool liIoAoVErY = false;
      bool omrAQHElgD = false;
      bool BqKgycobpt = false;
      bool EOREeRuFOR = false;
      bool NMCUSjSPUX = false;
      bool dkrMGuYaqX = false;
      bool ztOxTaUmUz = false;
      bool LyXOooopNN = false;
      bool KZGRQSVFyj = false;
      bool rUNpSTxEAr = false;
      bool PetVMeNdXR = false;
      bool GhNdMZnxfA = false;
      bool EPokGqkgwU = false;
      string qigkOSMDFN;
      string TsZHkpOQoT;
      string FwkGwfpSwE;
      string jbicmBNFEk;
      string zbKSofZAbB;
      string ZlaLetyZoh;
      string ctHAcrJxTj;
      string cJLDUdlSRQ;
      string FopteyAXxr;
      string DnQVHETKox;
      string WcgrYQZWqB;
      string IfIXcnEMVe;
      string qkAaforpwP;
      string JacTtcHxVN;
      string YcgujXersS;
      string AGoKFkcFLi;
      string XjaargCndA;
      string fGqHGNEEqI;
      string JAwcPDlRNJ;
      string zCklonCFDC;
      if(qigkOSMDFN == WcgrYQZWqB){UqOsmsPgEr = true;}
      else if(WcgrYQZWqB == qigkOSMDFN){EOREeRuFOR = true;}
      if(TsZHkpOQoT == IfIXcnEMVe){fXNDZXmyLn = true;}
      else if(IfIXcnEMVe == TsZHkpOQoT){NMCUSjSPUX = true;}
      if(FwkGwfpSwE == qkAaforpwP){tjZUAdUERR = true;}
      else if(qkAaforpwP == FwkGwfpSwE){dkrMGuYaqX = true;}
      if(jbicmBNFEk == JacTtcHxVN){LyBnmRkDpV = true;}
      else if(JacTtcHxVN == jbicmBNFEk){ztOxTaUmUz = true;}
      if(zbKSofZAbB == YcgujXersS){nFNVcbnhMR = true;}
      else if(YcgujXersS == zbKSofZAbB){LyXOooopNN = true;}
      if(ZlaLetyZoh == AGoKFkcFLi){BDtOHjQtsI = true;}
      else if(AGoKFkcFLi == ZlaLetyZoh){KZGRQSVFyj = true;}
      if(ctHAcrJxTj == XjaargCndA){XKPlfhWYPH = true;}
      else if(XjaargCndA == ctHAcrJxTj){rUNpSTxEAr = true;}
      if(cJLDUdlSRQ == fGqHGNEEqI){liIoAoVErY = true;}
      if(FopteyAXxr == JAwcPDlRNJ){omrAQHElgD = true;}
      if(DnQVHETKox == zCklonCFDC){BqKgycobpt = true;}
      while(fGqHGNEEqI == cJLDUdlSRQ){PetVMeNdXR = true;}
      while(JAwcPDlRNJ == JAwcPDlRNJ){GhNdMZnxfA = true;}
      while(zCklonCFDC == zCklonCFDC){EPokGqkgwU = true;}
      if(UqOsmsPgEr == true){UqOsmsPgEr = false;}
      if(fXNDZXmyLn == true){fXNDZXmyLn = false;}
      if(tjZUAdUERR == true){tjZUAdUERR = false;}
      if(LyBnmRkDpV == true){LyBnmRkDpV = false;}
      if(nFNVcbnhMR == true){nFNVcbnhMR = false;}
      if(BDtOHjQtsI == true){BDtOHjQtsI = false;}
      if(XKPlfhWYPH == true){XKPlfhWYPH = false;}
      if(liIoAoVErY == true){liIoAoVErY = false;}
      if(omrAQHElgD == true){omrAQHElgD = false;}
      if(BqKgycobpt == true){BqKgycobpt = false;}
      if(EOREeRuFOR == true){EOREeRuFOR = false;}
      if(NMCUSjSPUX == true){NMCUSjSPUX = false;}
      if(dkrMGuYaqX == true){dkrMGuYaqX = false;}
      if(ztOxTaUmUz == true){ztOxTaUmUz = false;}
      if(LyXOooopNN == true){LyXOooopNN = false;}
      if(KZGRQSVFyj == true){KZGRQSVFyj = false;}
      if(rUNpSTxEAr == true){rUNpSTxEAr = false;}
      if(PetVMeNdXR == true){PetVMeNdXR = false;}
      if(GhNdMZnxfA == true){GhNdMZnxfA = false;}
      if(EPokGqkgwU == true){EPokGqkgwU = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class INMUYHXPWT
{ 
  void gSgOPRSDOa()
  { 
      bool MjZlCWdllz = false;
      bool LLMSAYPtbZ = false;
      bool sxOxoPZIkS = false;
      bool ukgYlBSYdc = false;
      bool SwHfgCPkEc = false;
      bool QgTIaPUcwm = false;
      bool xUBTDZdmgz = false;
      bool GpQzGxoiQm = false;
      bool AnEIBNRXyW = false;
      bool fobpextnbG = false;
      bool XbEFVaPTBd = false;
      bool ZkMJmIQsRp = false;
      bool IGoKSRNaTJ = false;
      bool CBHJPXjzla = false;
      bool xxVUwtcSDW = false;
      bool fBVkyRmtlE = false;
      bool MoAraXdCsO = false;
      bool YRGqrKjUNX = false;
      bool ntWxcAeVLx = false;
      bool koMhNHxdsY = false;
      string AtjnArboal;
      string lgxYtVxwSo;
      string caCjgVpmpW;
      string wEcKERbxMW;
      string otkggAjNxw;
      string aLyctZqzGd;
      string BoyDphDlTc;
      string sziuAioVCg;
      string lbotwjFzQe;
      string ZbphUAugqz;
      string wxtTDabCbK;
      string jVxZrZzEHG;
      string bEVnCQJBaA;
      string nIdCjohUqU;
      string HDWFuEePpt;
      string AqZkXraFRl;
      string RaabWbqtpD;
      string rdMmyXgPsy;
      string lsLfzwhser;
      string NbDECfbuiT;
      if(AtjnArboal == wxtTDabCbK){MjZlCWdllz = true;}
      else if(wxtTDabCbK == AtjnArboal){XbEFVaPTBd = true;}
      if(lgxYtVxwSo == jVxZrZzEHG){LLMSAYPtbZ = true;}
      else if(jVxZrZzEHG == lgxYtVxwSo){ZkMJmIQsRp = true;}
      if(caCjgVpmpW == bEVnCQJBaA){sxOxoPZIkS = true;}
      else if(bEVnCQJBaA == caCjgVpmpW){IGoKSRNaTJ = true;}
      if(wEcKERbxMW == nIdCjohUqU){ukgYlBSYdc = true;}
      else if(nIdCjohUqU == wEcKERbxMW){CBHJPXjzla = true;}
      if(otkggAjNxw == HDWFuEePpt){SwHfgCPkEc = true;}
      else if(HDWFuEePpt == otkggAjNxw){xxVUwtcSDW = true;}
      if(aLyctZqzGd == AqZkXraFRl){QgTIaPUcwm = true;}
      else if(AqZkXraFRl == aLyctZqzGd){fBVkyRmtlE = true;}
      if(BoyDphDlTc == RaabWbqtpD){xUBTDZdmgz = true;}
      else if(RaabWbqtpD == BoyDphDlTc){MoAraXdCsO = true;}
      if(sziuAioVCg == rdMmyXgPsy){GpQzGxoiQm = true;}
      if(lbotwjFzQe == lsLfzwhser){AnEIBNRXyW = true;}
      if(ZbphUAugqz == NbDECfbuiT){fobpextnbG = true;}
      while(rdMmyXgPsy == sziuAioVCg){YRGqrKjUNX = true;}
      while(lsLfzwhser == lsLfzwhser){ntWxcAeVLx = true;}
      while(NbDECfbuiT == NbDECfbuiT){koMhNHxdsY = true;}
      if(MjZlCWdllz == true){MjZlCWdllz = false;}
      if(LLMSAYPtbZ == true){LLMSAYPtbZ = false;}
      if(sxOxoPZIkS == true){sxOxoPZIkS = false;}
      if(ukgYlBSYdc == true){ukgYlBSYdc = false;}
      if(SwHfgCPkEc == true){SwHfgCPkEc = false;}
      if(QgTIaPUcwm == true){QgTIaPUcwm = false;}
      if(xUBTDZdmgz == true){xUBTDZdmgz = false;}
      if(GpQzGxoiQm == true){GpQzGxoiQm = false;}
      if(AnEIBNRXyW == true){AnEIBNRXyW = false;}
      if(fobpextnbG == true){fobpextnbG = false;}
      if(XbEFVaPTBd == true){XbEFVaPTBd = false;}
      if(ZkMJmIQsRp == true){ZkMJmIQsRp = false;}
      if(IGoKSRNaTJ == true){IGoKSRNaTJ = false;}
      if(CBHJPXjzla == true){CBHJPXjzla = false;}
      if(xxVUwtcSDW == true){xxVUwtcSDW = false;}
      if(fBVkyRmtlE == true){fBVkyRmtlE = false;}
      if(MoAraXdCsO == true){MoAraXdCsO = false;}
      if(YRGqrKjUNX == true){YRGqrKjUNX = false;}
      if(ntWxcAeVLx == true){ntWxcAeVLx = false;}
      if(koMhNHxdsY == true){koMhNHxdsY = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JNHGODYZTU
{ 
  void mRTbXBKnrM()
  { 
      bool VUuholzOLS = false;
      bool RiCAUaTxsw = false;
      bool fYehonHgfb = false;
      bool dpwpCoucSG = false;
      bool hKTubjupZV = false;
      bool JCdxVwoMsA = false;
      bool dRiinalMyf = false;
      bool eqdlNVEjFE = false;
      bool rRGJRbLzTF = false;
      bool zhjScPajGA = false;
      bool xirtpSXriA = false;
      bool rEGnHMKYRM = false;
      bool PmrrfwGLJy = false;
      bool tUcFGJYMTy = false;
      bool ETPTMubwKP = false;
      bool ECRtkKpzNs = false;
      bool MLhkFXbBgy = false;
      bool myFTHwWYek = false;
      bool UKBssrCiUw = false;
      bool SZzZlskeOo = false;
      string YAwDYdZHmi;
      string zPMWaJjZRW;
      string FHVzbpKbYG;
      string YXRntEQzca;
      string uOLqHAKXru;
      string OfeqdQexbi;
      string SxhfJbyXcW;
      string rkxqapudqa;
      string fmjdqssZKc;
      string ZTXtHxffhf;
      string CltUDdinPm;
      string VAFACfNDoP;
      string hXMzxXNDic;
      string JPwLYssOaM;
      string KEKWHYOWqb;
      string jcpRxeZtXe;
      string QfElcFDsbI;
      string mycbFdNprX;
      string JpLHsgYeOs;
      string TypuwdzXsC;
      if(YAwDYdZHmi == CltUDdinPm){VUuholzOLS = true;}
      else if(CltUDdinPm == YAwDYdZHmi){xirtpSXriA = true;}
      if(zPMWaJjZRW == VAFACfNDoP){RiCAUaTxsw = true;}
      else if(VAFACfNDoP == zPMWaJjZRW){rEGnHMKYRM = true;}
      if(FHVzbpKbYG == hXMzxXNDic){fYehonHgfb = true;}
      else if(hXMzxXNDic == FHVzbpKbYG){PmrrfwGLJy = true;}
      if(YXRntEQzca == JPwLYssOaM){dpwpCoucSG = true;}
      else if(JPwLYssOaM == YXRntEQzca){tUcFGJYMTy = true;}
      if(uOLqHAKXru == KEKWHYOWqb){hKTubjupZV = true;}
      else if(KEKWHYOWqb == uOLqHAKXru){ETPTMubwKP = true;}
      if(OfeqdQexbi == jcpRxeZtXe){JCdxVwoMsA = true;}
      else if(jcpRxeZtXe == OfeqdQexbi){ECRtkKpzNs = true;}
      if(SxhfJbyXcW == QfElcFDsbI){dRiinalMyf = true;}
      else if(QfElcFDsbI == SxhfJbyXcW){MLhkFXbBgy = true;}
      if(rkxqapudqa == mycbFdNprX){eqdlNVEjFE = true;}
      if(fmjdqssZKc == JpLHsgYeOs){rRGJRbLzTF = true;}
      if(ZTXtHxffhf == TypuwdzXsC){zhjScPajGA = true;}
      while(mycbFdNprX == rkxqapudqa){myFTHwWYek = true;}
      while(JpLHsgYeOs == JpLHsgYeOs){UKBssrCiUw = true;}
      while(TypuwdzXsC == TypuwdzXsC){SZzZlskeOo = true;}
      if(VUuholzOLS == true){VUuholzOLS = false;}
      if(RiCAUaTxsw == true){RiCAUaTxsw = false;}
      if(fYehonHgfb == true){fYehonHgfb = false;}
      if(dpwpCoucSG == true){dpwpCoucSG = false;}
      if(hKTubjupZV == true){hKTubjupZV = false;}
      if(JCdxVwoMsA == true){JCdxVwoMsA = false;}
      if(dRiinalMyf == true){dRiinalMyf = false;}
      if(eqdlNVEjFE == true){eqdlNVEjFE = false;}
      if(rRGJRbLzTF == true){rRGJRbLzTF = false;}
      if(zhjScPajGA == true){zhjScPajGA = false;}
      if(xirtpSXriA == true){xirtpSXriA = false;}
      if(rEGnHMKYRM == true){rEGnHMKYRM = false;}
      if(PmrrfwGLJy == true){PmrrfwGLJy = false;}
      if(tUcFGJYMTy == true){tUcFGJYMTy = false;}
      if(ETPTMubwKP == true){ETPTMubwKP = false;}
      if(ECRtkKpzNs == true){ECRtkKpzNs = false;}
      if(MLhkFXbBgy == true){MLhkFXbBgy = false;}
      if(myFTHwWYek == true){myFTHwWYek = false;}
      if(UKBssrCiUw == true){UKBssrCiUw = false;}
      if(SZzZlskeOo == true){SZzZlskeOo = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FCVJHZHPKD
{ 
  void VwWmVmHUMl()
  { 
      bool sGyIISqFYX = false;
      bool WZeLWFpZjU = false;
      bool GGUfcWjmtx = false;
      bool piuzJbfGuV = false;
      bool GPlAAKQcOt = false;
      bool awXZElkSel = false;
      bool hDKbgnefBu = false;
      bool ZjdNOpUUeS = false;
      bool FGYAWtKVtW = false;
      bool qNaJmCFKTM = false;
      bool ekbTUpRJLN = false;
      bool RQnMfaeKFm = false;
      bool cJkeDMJTZL = false;
      bool kJBSnUeFef = false;
      bool bBJgEaZOdL = false;
      bool XtEfiUAxPI = false;
      bool gxizcIPUXK = false;
      bool wYeZKbzDZc = false;
      bool mtpNmpFUiS = false;
      bool UhoVydRJDw = false;
      string SqocXYPEaf;
      string EycRPycsVe;
      string fSzeQntFFq;
      string efodjVMEUK;
      string dhPZWsTUXY;
      string oPUEgAqKMi;
      string dKyiAYIzgs;
      string kXXLAyCYHW;
      string UZgfCOKkwj;
      string dCqkfTigZy;
      string kTmdnQrJEh;
      string xRCoOEcRfk;
      string NWDxPhglef;
      string PVzJlAlDrk;
      string WAfNlLzCWa;
      string qRAnkNPpOF;
      string jFdYSaSsDN;
      string SYZuOMppKD;
      string hePEnyULXX;
      string jwolAQVHtK;
      if(SqocXYPEaf == kTmdnQrJEh){sGyIISqFYX = true;}
      else if(kTmdnQrJEh == SqocXYPEaf){ekbTUpRJLN = true;}
      if(EycRPycsVe == xRCoOEcRfk){WZeLWFpZjU = true;}
      else if(xRCoOEcRfk == EycRPycsVe){RQnMfaeKFm = true;}
      if(fSzeQntFFq == NWDxPhglef){GGUfcWjmtx = true;}
      else if(NWDxPhglef == fSzeQntFFq){cJkeDMJTZL = true;}
      if(efodjVMEUK == PVzJlAlDrk){piuzJbfGuV = true;}
      else if(PVzJlAlDrk == efodjVMEUK){kJBSnUeFef = true;}
      if(dhPZWsTUXY == WAfNlLzCWa){GPlAAKQcOt = true;}
      else if(WAfNlLzCWa == dhPZWsTUXY){bBJgEaZOdL = true;}
      if(oPUEgAqKMi == qRAnkNPpOF){awXZElkSel = true;}
      else if(qRAnkNPpOF == oPUEgAqKMi){XtEfiUAxPI = true;}
      if(dKyiAYIzgs == jFdYSaSsDN){hDKbgnefBu = true;}
      else if(jFdYSaSsDN == dKyiAYIzgs){gxizcIPUXK = true;}
      if(kXXLAyCYHW == SYZuOMppKD){ZjdNOpUUeS = true;}
      if(UZgfCOKkwj == hePEnyULXX){FGYAWtKVtW = true;}
      if(dCqkfTigZy == jwolAQVHtK){qNaJmCFKTM = true;}
      while(SYZuOMppKD == kXXLAyCYHW){wYeZKbzDZc = true;}
      while(hePEnyULXX == hePEnyULXX){mtpNmpFUiS = true;}
      while(jwolAQVHtK == jwolAQVHtK){UhoVydRJDw = true;}
      if(sGyIISqFYX == true){sGyIISqFYX = false;}
      if(WZeLWFpZjU == true){WZeLWFpZjU = false;}
      if(GGUfcWjmtx == true){GGUfcWjmtx = false;}
      if(piuzJbfGuV == true){piuzJbfGuV = false;}
      if(GPlAAKQcOt == true){GPlAAKQcOt = false;}
      if(awXZElkSel == true){awXZElkSel = false;}
      if(hDKbgnefBu == true){hDKbgnefBu = false;}
      if(ZjdNOpUUeS == true){ZjdNOpUUeS = false;}
      if(FGYAWtKVtW == true){FGYAWtKVtW = false;}
      if(qNaJmCFKTM == true){qNaJmCFKTM = false;}
      if(ekbTUpRJLN == true){ekbTUpRJLN = false;}
      if(RQnMfaeKFm == true){RQnMfaeKFm = false;}
      if(cJkeDMJTZL == true){cJkeDMJTZL = false;}
      if(kJBSnUeFef == true){kJBSnUeFef = false;}
      if(bBJgEaZOdL == true){bBJgEaZOdL = false;}
      if(XtEfiUAxPI == true){XtEfiUAxPI = false;}
      if(gxizcIPUXK == true){gxizcIPUXK = false;}
      if(wYeZKbzDZc == true){wYeZKbzDZc = false;}
      if(mtpNmpFUiS == true){mtpNmpFUiS = false;}
      if(UhoVydRJDw == true){UhoVydRJDw = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class USXNXIFPTQ
{ 
  void DYCVHBPVwT()
  { 
      bool yuAymfyWyK = false;
      bool FDEqbZWBQt = false;
      bool KqNWmnzBTb = false;
      bool ZFkZPcNWBO = false;
      bool fnBdPtrZfK = false;
      bool jOmPTGhcKz = false;
      bool gxCWkzSdbz = false;
      bool MAACOVpiHs = false;
      bool uZUJrKRVFI = false;
      bool ujLBgikerP = false;
      bool bOxgxjOkBy = false;
      bool QyVRDGbsda = false;
      bool hwYTDbnyUe = false;
      bool kKprRgbnLI = false;
      bool ugUHqwOtyZ = false;
      bool GrARheodcV = false;
      bool uPQVMGHdzW = false;
      bool engVeAbQbH = false;
      bool NtAtYsKYJm = false;
      bool qxKpMeQztk = false;
      string mLbKuyZNmu;
      string qpzOFlCFwd;
      string opotGipHOq;
      string RkxXElYYfV;
      string CZArEgGXbi;
      string XVbebxWhDL;
      string IRywcGraKS;
      string McYZbOpxoZ;
      string cRefSuISwG;
      string PqVTNjbEOy;
      string oeMIWuxCDM;
      string HZscipmScg;
      string xYeEDVGFLq;
      string mlhpkxFVJX;
      string yGPizpxsjX;
      string xIahBPzIFA;
      string nqpLYUqCrl;
      string jQJEipJuEN;
      string XNUnteKAoU;
      string ywQIjGQEJd;
      if(mLbKuyZNmu == oeMIWuxCDM){yuAymfyWyK = true;}
      else if(oeMIWuxCDM == mLbKuyZNmu){bOxgxjOkBy = true;}
      if(qpzOFlCFwd == HZscipmScg){FDEqbZWBQt = true;}
      else if(HZscipmScg == qpzOFlCFwd){QyVRDGbsda = true;}
      if(opotGipHOq == xYeEDVGFLq){KqNWmnzBTb = true;}
      else if(xYeEDVGFLq == opotGipHOq){hwYTDbnyUe = true;}
      if(RkxXElYYfV == mlhpkxFVJX){ZFkZPcNWBO = true;}
      else if(mlhpkxFVJX == RkxXElYYfV){kKprRgbnLI = true;}
      if(CZArEgGXbi == yGPizpxsjX){fnBdPtrZfK = true;}
      else if(yGPizpxsjX == CZArEgGXbi){ugUHqwOtyZ = true;}
      if(XVbebxWhDL == xIahBPzIFA){jOmPTGhcKz = true;}
      else if(xIahBPzIFA == XVbebxWhDL){GrARheodcV = true;}
      if(IRywcGraKS == nqpLYUqCrl){gxCWkzSdbz = true;}
      else if(nqpLYUqCrl == IRywcGraKS){uPQVMGHdzW = true;}
      if(McYZbOpxoZ == jQJEipJuEN){MAACOVpiHs = true;}
      if(cRefSuISwG == XNUnteKAoU){uZUJrKRVFI = true;}
      if(PqVTNjbEOy == ywQIjGQEJd){ujLBgikerP = true;}
      while(jQJEipJuEN == McYZbOpxoZ){engVeAbQbH = true;}
      while(XNUnteKAoU == XNUnteKAoU){NtAtYsKYJm = true;}
      while(ywQIjGQEJd == ywQIjGQEJd){qxKpMeQztk = true;}
      if(yuAymfyWyK == true){yuAymfyWyK = false;}
      if(FDEqbZWBQt == true){FDEqbZWBQt = false;}
      if(KqNWmnzBTb == true){KqNWmnzBTb = false;}
      if(ZFkZPcNWBO == true){ZFkZPcNWBO = false;}
      if(fnBdPtrZfK == true){fnBdPtrZfK = false;}
      if(jOmPTGhcKz == true){jOmPTGhcKz = false;}
      if(gxCWkzSdbz == true){gxCWkzSdbz = false;}
      if(MAACOVpiHs == true){MAACOVpiHs = false;}
      if(uZUJrKRVFI == true){uZUJrKRVFI = false;}
      if(ujLBgikerP == true){ujLBgikerP = false;}
      if(bOxgxjOkBy == true){bOxgxjOkBy = false;}
      if(QyVRDGbsda == true){QyVRDGbsda = false;}
      if(hwYTDbnyUe == true){hwYTDbnyUe = false;}
      if(kKprRgbnLI == true){kKprRgbnLI = false;}
      if(ugUHqwOtyZ == true){ugUHqwOtyZ = false;}
      if(GrARheodcV == true){GrARheodcV = false;}
      if(uPQVMGHdzW == true){uPQVMGHdzW = false;}
      if(engVeAbQbH == true){engVeAbQbH = false;}
      if(NtAtYsKYJm == true){NtAtYsKYJm = false;}
      if(qxKpMeQztk == true){qxKpMeQztk = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UVDGCOVYEU
{ 
  void NIhhzRinZT()
  { 
      bool acnTBTqESJ = false;
      bool QFfcmNQoOz = false;
      bool afYQpsCAIS = false;
      bool KFhWWdhLyg = false;
      bool cSmahDmWky = false;
      bool ZkcyHuDXcI = false;
      bool RpuAyKWIFI = false;
      bool eFpgVHryhl = false;
      bool GePJsYcjPb = false;
      bool aKhpPmXlbW = false;
      bool geQbTTuucA = false;
      bool CEopBmZlHY = false;
      bool hmrdhUWARh = false;
      bool iFWLFaNnpY = false;
      bool dhGOYVVrnc = false;
      bool NleDotlmsy = false;
      bool xuctQwhdja = false;
      bool YkuhtzfdEL = false;
      bool zeLoUxaTVL = false;
      bool oatqbiEMxc = false;
      string kyDKFOmLES;
      string mEmYNGDsVJ;
      string xDRpAhSxNe;
      string mSDSJRmrib;
      string nFQGQTEniH;
      string VEyIwhgNKW;
      string upyZfUAUOw;
      string SKJtUrmKZg;
      string ddNGIhTuVb;
      string jZhLfyHpau;
      string SjYmnLtDmE;
      string tWmdsRbqRx;
      string BFymLsbyOW;
      string jwjdNtEaSw;
      string mPHRgNlESO;
      string VPYctNklat;
      string jwoQTylJPz;
      string MBYUJkxfGy;
      string HWeABUXGjz;
      string ISzXMqZqxi;
      if(kyDKFOmLES == SjYmnLtDmE){acnTBTqESJ = true;}
      else if(SjYmnLtDmE == kyDKFOmLES){geQbTTuucA = true;}
      if(mEmYNGDsVJ == tWmdsRbqRx){QFfcmNQoOz = true;}
      else if(tWmdsRbqRx == mEmYNGDsVJ){CEopBmZlHY = true;}
      if(xDRpAhSxNe == BFymLsbyOW){afYQpsCAIS = true;}
      else if(BFymLsbyOW == xDRpAhSxNe){hmrdhUWARh = true;}
      if(mSDSJRmrib == jwjdNtEaSw){KFhWWdhLyg = true;}
      else if(jwjdNtEaSw == mSDSJRmrib){iFWLFaNnpY = true;}
      if(nFQGQTEniH == mPHRgNlESO){cSmahDmWky = true;}
      else if(mPHRgNlESO == nFQGQTEniH){dhGOYVVrnc = true;}
      if(VEyIwhgNKW == VPYctNklat){ZkcyHuDXcI = true;}
      else if(VPYctNklat == VEyIwhgNKW){NleDotlmsy = true;}
      if(upyZfUAUOw == jwoQTylJPz){RpuAyKWIFI = true;}
      else if(jwoQTylJPz == upyZfUAUOw){xuctQwhdja = true;}
      if(SKJtUrmKZg == MBYUJkxfGy){eFpgVHryhl = true;}
      if(ddNGIhTuVb == HWeABUXGjz){GePJsYcjPb = true;}
      if(jZhLfyHpau == ISzXMqZqxi){aKhpPmXlbW = true;}
      while(MBYUJkxfGy == SKJtUrmKZg){YkuhtzfdEL = true;}
      while(HWeABUXGjz == HWeABUXGjz){zeLoUxaTVL = true;}
      while(ISzXMqZqxi == ISzXMqZqxi){oatqbiEMxc = true;}
      if(acnTBTqESJ == true){acnTBTqESJ = false;}
      if(QFfcmNQoOz == true){QFfcmNQoOz = false;}
      if(afYQpsCAIS == true){afYQpsCAIS = false;}
      if(KFhWWdhLyg == true){KFhWWdhLyg = false;}
      if(cSmahDmWky == true){cSmahDmWky = false;}
      if(ZkcyHuDXcI == true){ZkcyHuDXcI = false;}
      if(RpuAyKWIFI == true){RpuAyKWIFI = false;}
      if(eFpgVHryhl == true){eFpgVHryhl = false;}
      if(GePJsYcjPb == true){GePJsYcjPb = false;}
      if(aKhpPmXlbW == true){aKhpPmXlbW = false;}
      if(geQbTTuucA == true){geQbTTuucA = false;}
      if(CEopBmZlHY == true){CEopBmZlHY = false;}
      if(hmrdhUWARh == true){hmrdhUWARh = false;}
      if(iFWLFaNnpY == true){iFWLFaNnpY = false;}
      if(dhGOYVVrnc == true){dhGOYVVrnc = false;}
      if(NleDotlmsy == true){NleDotlmsy = false;}
      if(xuctQwhdja == true){xuctQwhdja = false;}
      if(YkuhtzfdEL == true){YkuhtzfdEL = false;}
      if(zeLoUxaTVL == true){zeLoUxaTVL = false;}
      if(oatqbiEMxc == true){oatqbiEMxc = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class OZDYWSCCKF
{ 
  void QEyjbzQgyu()
  { 
      bool SdCNiSicec = false;
      bool RTpFKjbUuQ = false;
      bool ONAVhIyPNc = false;
      bool ugCfRQAPee = false;
      bool PBSjbVKqRG = false;
      bool AyFchFcffa = false;
      bool TSPChjRJnt = false;
      bool iMoPANtQtM = false;
      bool xHyHbxaZxL = false;
      bool mUjELxkQBq = false;
      bool DACWWnIpbU = false;
      bool KMlScLTHZj = false;
      bool JrMcGOcjED = false;
      bool bdOCIwEajn = false;
      bool nkxcAahQbz = false;
      bool cdpRhLnZPI = false;
      bool LYlYsgxiJf = false;
      bool HrXlwXtibf = false;
      bool fLHEZymxUx = false;
      bool IeLbbPzXiT = false;
      string woBGCkOLLz;
      string RsWoXiAxei;
      string ZOgYbzCmNq;
      string FKPRchlFSD;
      string YylVNGYytn;
      string EASORhHceD;
      string hbyUtHcEMo;
      string wuebJacLKV;
      string ZErYlEqAzt;
      string aJaOUxAQaG;
      string cUkErIVpFp;
      string FwRIcciYho;
      string PcEROmUPGL;
      string aMjDsFTmgd;
      string iBtIoerRlr;
      string OahlqzPSpF;
      string mFisPrLNPX;
      string JurEGNhucj;
      string tDdOxzdyMs;
      string oAxbpcSBsw;
      if(woBGCkOLLz == cUkErIVpFp){SdCNiSicec = true;}
      else if(cUkErIVpFp == woBGCkOLLz){DACWWnIpbU = true;}
      if(RsWoXiAxei == FwRIcciYho){RTpFKjbUuQ = true;}
      else if(FwRIcciYho == RsWoXiAxei){KMlScLTHZj = true;}
      if(ZOgYbzCmNq == PcEROmUPGL){ONAVhIyPNc = true;}
      else if(PcEROmUPGL == ZOgYbzCmNq){JrMcGOcjED = true;}
      if(FKPRchlFSD == aMjDsFTmgd){ugCfRQAPee = true;}
      else if(aMjDsFTmgd == FKPRchlFSD){bdOCIwEajn = true;}
      if(YylVNGYytn == iBtIoerRlr){PBSjbVKqRG = true;}
      else if(iBtIoerRlr == YylVNGYytn){nkxcAahQbz = true;}
      if(EASORhHceD == OahlqzPSpF){AyFchFcffa = true;}
      else if(OahlqzPSpF == EASORhHceD){cdpRhLnZPI = true;}
      if(hbyUtHcEMo == mFisPrLNPX){TSPChjRJnt = true;}
      else if(mFisPrLNPX == hbyUtHcEMo){LYlYsgxiJf = true;}
      if(wuebJacLKV == JurEGNhucj){iMoPANtQtM = true;}
      if(ZErYlEqAzt == tDdOxzdyMs){xHyHbxaZxL = true;}
      if(aJaOUxAQaG == oAxbpcSBsw){mUjELxkQBq = true;}
      while(JurEGNhucj == wuebJacLKV){HrXlwXtibf = true;}
      while(tDdOxzdyMs == tDdOxzdyMs){fLHEZymxUx = true;}
      while(oAxbpcSBsw == oAxbpcSBsw){IeLbbPzXiT = true;}
      if(SdCNiSicec == true){SdCNiSicec = false;}
      if(RTpFKjbUuQ == true){RTpFKjbUuQ = false;}
      if(ONAVhIyPNc == true){ONAVhIyPNc = false;}
      if(ugCfRQAPee == true){ugCfRQAPee = false;}
      if(PBSjbVKqRG == true){PBSjbVKqRG = false;}
      if(AyFchFcffa == true){AyFchFcffa = false;}
      if(TSPChjRJnt == true){TSPChjRJnt = false;}
      if(iMoPANtQtM == true){iMoPANtQtM = false;}
      if(xHyHbxaZxL == true){xHyHbxaZxL = false;}
      if(mUjELxkQBq == true){mUjELxkQBq = false;}
      if(DACWWnIpbU == true){DACWWnIpbU = false;}
      if(KMlScLTHZj == true){KMlScLTHZj = false;}
      if(JrMcGOcjED == true){JrMcGOcjED = false;}
      if(bdOCIwEajn == true){bdOCIwEajn = false;}
      if(nkxcAahQbz == true){nkxcAahQbz = false;}
      if(cdpRhLnZPI == true){cdpRhLnZPI = false;}
      if(LYlYsgxiJf == true){LYlYsgxiJf = false;}
      if(HrXlwXtibf == true){HrXlwXtibf = false;}
      if(fLHEZymxUx == true){fLHEZymxUx = false;}
      if(IeLbbPzXiT == true){IeLbbPzXiT = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ROBOFYTUAE
{ 
  void fdFuXnjKbk()
  { 
      bool SRYgVYnZXs = false;
      bool desIcrTLga = false;
      bool xkjYMlMpYr = false;
      bool OjlgeQxBeL = false;
      bool eZQFirrHfY = false;
      bool tjeIkggAUi = false;
      bool MyBfessMcM = false;
      bool tRgUuWUeoU = false;
      bool sEKzxdFcux = false;
      bool ixguzfXkZp = false;
      bool yfeTebCJMO = false;
      bool MCKzpHmChA = false;
      bool nQRecwklVP = false;
      bool ziEzVNZqUU = false;
      bool auPgkgmgFr = false;
      bool sUCZBhOzqh = false;
      bool qQnxOdxKdb = false;
      bool gwhMYsBjtu = false;
      bool JDgcVdfeyn = false;
      bool zazldKGRRG = false;
      string BlsZhITQtu;
      string MhXypVcklD;
      string HkTgGAqBgw;
      string CdXtJHzGId;
      string RrjwGNcsfQ;
      string SfOoQPSMTg;
      string QSjysiKcoh;
      string VVbHkPBWar;
      string XyUPcVPids;
      string ommaMzIkCW;
      string DzDytxEGDA;
      string NulaJSyTUX;
      string SbyurihBtk;
      string HQTDoYtVUf;
      string RIecftALRt;
      string AnPeEaAIGL;
      string OCMRsxailj;
      string XbzoGKxUPy;
      string hWiAkSAoyN;
      string XlPIQOwMih;
      if(BlsZhITQtu == DzDytxEGDA){SRYgVYnZXs = true;}
      else if(DzDytxEGDA == BlsZhITQtu){yfeTebCJMO = true;}
      if(MhXypVcklD == NulaJSyTUX){desIcrTLga = true;}
      else if(NulaJSyTUX == MhXypVcklD){MCKzpHmChA = true;}
      if(HkTgGAqBgw == SbyurihBtk){xkjYMlMpYr = true;}
      else if(SbyurihBtk == HkTgGAqBgw){nQRecwklVP = true;}
      if(CdXtJHzGId == HQTDoYtVUf){OjlgeQxBeL = true;}
      else if(HQTDoYtVUf == CdXtJHzGId){ziEzVNZqUU = true;}
      if(RrjwGNcsfQ == RIecftALRt){eZQFirrHfY = true;}
      else if(RIecftALRt == RrjwGNcsfQ){auPgkgmgFr = true;}
      if(SfOoQPSMTg == AnPeEaAIGL){tjeIkggAUi = true;}
      else if(AnPeEaAIGL == SfOoQPSMTg){sUCZBhOzqh = true;}
      if(QSjysiKcoh == OCMRsxailj){MyBfessMcM = true;}
      else if(OCMRsxailj == QSjysiKcoh){qQnxOdxKdb = true;}
      if(VVbHkPBWar == XbzoGKxUPy){tRgUuWUeoU = true;}
      if(XyUPcVPids == hWiAkSAoyN){sEKzxdFcux = true;}
      if(ommaMzIkCW == XlPIQOwMih){ixguzfXkZp = true;}
      while(XbzoGKxUPy == VVbHkPBWar){gwhMYsBjtu = true;}
      while(hWiAkSAoyN == hWiAkSAoyN){JDgcVdfeyn = true;}
      while(XlPIQOwMih == XlPIQOwMih){zazldKGRRG = true;}
      if(SRYgVYnZXs == true){SRYgVYnZXs = false;}
      if(desIcrTLga == true){desIcrTLga = false;}
      if(xkjYMlMpYr == true){xkjYMlMpYr = false;}
      if(OjlgeQxBeL == true){OjlgeQxBeL = false;}
      if(eZQFirrHfY == true){eZQFirrHfY = false;}
      if(tjeIkggAUi == true){tjeIkggAUi = false;}
      if(MyBfessMcM == true){MyBfessMcM = false;}
      if(tRgUuWUeoU == true){tRgUuWUeoU = false;}
      if(sEKzxdFcux == true){sEKzxdFcux = false;}
      if(ixguzfXkZp == true){ixguzfXkZp = false;}
      if(yfeTebCJMO == true){yfeTebCJMO = false;}
      if(MCKzpHmChA == true){MCKzpHmChA = false;}
      if(nQRecwklVP == true){nQRecwklVP = false;}
      if(ziEzVNZqUU == true){ziEzVNZqUU = false;}
      if(auPgkgmgFr == true){auPgkgmgFr = false;}
      if(sUCZBhOzqh == true){sUCZBhOzqh = false;}
      if(qQnxOdxKdb == true){qQnxOdxKdb = false;}
      if(gwhMYsBjtu == true){gwhMYsBjtu = false;}
      if(JDgcVdfeyn == true){JDgcVdfeyn = false;}
      if(zazldKGRRG == true){zazldKGRRG = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class GMOHMPNMBP
{ 
  void AuWhnRomKB()
  { 
      bool LFFZWWoSiF = false;
      bool AsSPtSXNMc = false;
      bool bKNonRabnj = false;
      bool KNbpwqSzRN = false;
      bool WVTwOLEAWS = false;
      bool GjCLAArPkF = false;
      bool uNdztmRcSD = false;
      bool zXuzGFqQab = false;
      bool NaFUcaKPou = false;
      bool yDNMRIrUBa = false;
      bool tagTThnDFt = false;
      bool JaoIrTIJVS = false;
      bool bCmXEwKigx = false;
      bool NVPzhltlBu = false;
      bool mHJulXNETx = false;
      bool AhzhzrnJOW = false;
      bool XEVHgnLoaF = false;
      bool ZVIZfTrrlg = false;
      bool iFOiweVgAo = false;
      bool ykWeWhlCNx = false;
      string PilSMRFGej;
      string kniZQdzdeQ;
      string dlfVPiKcoV;
      string dWtZVrNJHR;
      string XAGrkINCfY;
      string QMRnMdCPDd;
      string GcRJGNsIEY;
      string PoWMscemkG;
      string UHwQEQFGNr;
      string oyGcKtixlW;
      string PFyHIapEui;
      string yOdwJyfAEo;
      string ImuMiRisBy;
      string thAOqPJoHd;
      string aAKdsZUKTM;
      string OhsxNbJlYq;
      string qwCwoQOOEt;
      string oqsZNIbQSI;
      string wYYojLoDBO;
      string FWPQldxOoc;
      if(PilSMRFGej == PFyHIapEui){LFFZWWoSiF = true;}
      else if(PFyHIapEui == PilSMRFGej){tagTThnDFt = true;}
      if(kniZQdzdeQ == yOdwJyfAEo){AsSPtSXNMc = true;}
      else if(yOdwJyfAEo == kniZQdzdeQ){JaoIrTIJVS = true;}
      if(dlfVPiKcoV == ImuMiRisBy){bKNonRabnj = true;}
      else if(ImuMiRisBy == dlfVPiKcoV){bCmXEwKigx = true;}
      if(dWtZVrNJHR == thAOqPJoHd){KNbpwqSzRN = true;}
      else if(thAOqPJoHd == dWtZVrNJHR){NVPzhltlBu = true;}
      if(XAGrkINCfY == aAKdsZUKTM){WVTwOLEAWS = true;}
      else if(aAKdsZUKTM == XAGrkINCfY){mHJulXNETx = true;}
      if(QMRnMdCPDd == OhsxNbJlYq){GjCLAArPkF = true;}
      else if(OhsxNbJlYq == QMRnMdCPDd){AhzhzrnJOW = true;}
      if(GcRJGNsIEY == qwCwoQOOEt){uNdztmRcSD = true;}
      else if(qwCwoQOOEt == GcRJGNsIEY){XEVHgnLoaF = true;}
      if(PoWMscemkG == oqsZNIbQSI){zXuzGFqQab = true;}
      if(UHwQEQFGNr == wYYojLoDBO){NaFUcaKPou = true;}
      if(oyGcKtixlW == FWPQldxOoc){yDNMRIrUBa = true;}
      while(oqsZNIbQSI == PoWMscemkG){ZVIZfTrrlg = true;}
      while(wYYojLoDBO == wYYojLoDBO){iFOiweVgAo = true;}
      while(FWPQldxOoc == FWPQldxOoc){ykWeWhlCNx = true;}
      if(LFFZWWoSiF == true){LFFZWWoSiF = false;}
      if(AsSPtSXNMc == true){AsSPtSXNMc = false;}
      if(bKNonRabnj == true){bKNonRabnj = false;}
      if(KNbpwqSzRN == true){KNbpwqSzRN = false;}
      if(WVTwOLEAWS == true){WVTwOLEAWS = false;}
      if(GjCLAArPkF == true){GjCLAArPkF = false;}
      if(uNdztmRcSD == true){uNdztmRcSD = false;}
      if(zXuzGFqQab == true){zXuzGFqQab = false;}
      if(NaFUcaKPou == true){NaFUcaKPou = false;}
      if(yDNMRIrUBa == true){yDNMRIrUBa = false;}
      if(tagTThnDFt == true){tagTThnDFt = false;}
      if(JaoIrTIJVS == true){JaoIrTIJVS = false;}
      if(bCmXEwKigx == true){bCmXEwKigx = false;}
      if(NVPzhltlBu == true){NVPzhltlBu = false;}
      if(mHJulXNETx == true){mHJulXNETx = false;}
      if(AhzhzrnJOW == true){AhzhzrnJOW = false;}
      if(XEVHgnLoaF == true){XEVHgnLoaF = false;}
      if(ZVIZfTrrlg == true){ZVIZfTrrlg = false;}
      if(iFOiweVgAo == true){iFOiweVgAo = false;}
      if(ykWeWhlCNx == true){ykWeWhlCNx = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VXNBDYJRMO
{ 
  void ZaPBAAmNBa()
  { 
      bool wXzZPgSIBX = false;
      bool WgwsdCuoHr = false;
      bool iDjCXuVpFp = false;
      bool mplWssYanU = false;
      bool jPEUGNWLPI = false;
      bool gaxGDsmxCT = false;
      bool KcDLjgtYAk = false;
      bool dTAmITyPuJ = false;
      bool XKZxIMrfTz = false;
      bool FaYUJHLsjZ = false;
      bool TyLeeMaKyi = false;
      bool uXBsuACFCX = false;
      bool nNEAfCufcA = false;
      bool HFfVoZyuRu = false;
      bool kghkEhZeYI = false;
      bool EGpLkFtKzc = false;
      bool QthxbpqeDV = false;
      bool BHnHlHfTuy = false;
      bool bQHTwBOTki = false;
      bool fnwodqzjIO = false;
      string DdCmfPRNRg;
      string JQOkAVMimd;
      string sVHwontkiA;
      string YLZkhNEDQm;
      string BItTmRbMRe;
      string kIatmhWGyr;
      string VgdQHxpdDl;
      string QISAckbmJW;
      string LhSDOPjfxz;
      string pyFYRUdcMo;
      string byIeNcpaRC;
      string GjxafmNaCr;
      string MJfDwSCshI;
      string GyaTuNYmpk;
      string YJOtsGBerd;
      string GDZTbRSFiu;
      string ZwsJuHJceS;
      string OZjQSDukJp;
      string QXBDBDXGrc;
      string uSEtKcPFyl;
      if(DdCmfPRNRg == byIeNcpaRC){wXzZPgSIBX = true;}
      else if(byIeNcpaRC == DdCmfPRNRg){TyLeeMaKyi = true;}
      if(JQOkAVMimd == GjxafmNaCr){WgwsdCuoHr = true;}
      else if(GjxafmNaCr == JQOkAVMimd){uXBsuACFCX = true;}
      if(sVHwontkiA == MJfDwSCshI){iDjCXuVpFp = true;}
      else if(MJfDwSCshI == sVHwontkiA){nNEAfCufcA = true;}
      if(YLZkhNEDQm == GyaTuNYmpk){mplWssYanU = true;}
      else if(GyaTuNYmpk == YLZkhNEDQm){HFfVoZyuRu = true;}
      if(BItTmRbMRe == YJOtsGBerd){jPEUGNWLPI = true;}
      else if(YJOtsGBerd == BItTmRbMRe){kghkEhZeYI = true;}
      if(kIatmhWGyr == GDZTbRSFiu){gaxGDsmxCT = true;}
      else if(GDZTbRSFiu == kIatmhWGyr){EGpLkFtKzc = true;}
      if(VgdQHxpdDl == ZwsJuHJceS){KcDLjgtYAk = true;}
      else if(ZwsJuHJceS == VgdQHxpdDl){QthxbpqeDV = true;}
      if(QISAckbmJW == OZjQSDukJp){dTAmITyPuJ = true;}
      if(LhSDOPjfxz == QXBDBDXGrc){XKZxIMrfTz = true;}
      if(pyFYRUdcMo == uSEtKcPFyl){FaYUJHLsjZ = true;}
      while(OZjQSDukJp == QISAckbmJW){BHnHlHfTuy = true;}
      while(QXBDBDXGrc == QXBDBDXGrc){bQHTwBOTki = true;}
      while(uSEtKcPFyl == uSEtKcPFyl){fnwodqzjIO = true;}
      if(wXzZPgSIBX == true){wXzZPgSIBX = false;}
      if(WgwsdCuoHr == true){WgwsdCuoHr = false;}
      if(iDjCXuVpFp == true){iDjCXuVpFp = false;}
      if(mplWssYanU == true){mplWssYanU = false;}
      if(jPEUGNWLPI == true){jPEUGNWLPI = false;}
      if(gaxGDsmxCT == true){gaxGDsmxCT = false;}
      if(KcDLjgtYAk == true){KcDLjgtYAk = false;}
      if(dTAmITyPuJ == true){dTAmITyPuJ = false;}
      if(XKZxIMrfTz == true){XKZxIMrfTz = false;}
      if(FaYUJHLsjZ == true){FaYUJHLsjZ = false;}
      if(TyLeeMaKyi == true){TyLeeMaKyi = false;}
      if(uXBsuACFCX == true){uXBsuACFCX = false;}
      if(nNEAfCufcA == true){nNEAfCufcA = false;}
      if(HFfVoZyuRu == true){HFfVoZyuRu = false;}
      if(kghkEhZeYI == true){kghkEhZeYI = false;}
      if(EGpLkFtKzc == true){EGpLkFtKzc = false;}
      if(QthxbpqeDV == true){QthxbpqeDV = false;}
      if(BHnHlHfTuy == true){BHnHlHfTuy = false;}
      if(bQHTwBOTki == true){bQHTwBOTki = false;}
      if(fnwodqzjIO == true){fnwodqzjIO = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VLUBTDIOYQ
{ 
  void BqPFnhOLhP()
  { 
      bool YXdoPNoTYG = false;
      bool WcTsTuotOG = false;
      bool jcltaBXXpn = false;
      bool ucXmweiGgh = false;
      bool NFdnWnFxdj = false;
      bool kOrwpukiON = false;
      bool pQckLtEfwo = false;
      bool LiMfsdzyXl = false;
      bool uasGDfjbxB = false;
      bool szqSrwcrbb = false;
      bool iYAoXCBLCL = false;
      bool wmLcSPWWhe = false;
      bool JciuuLqsnw = false;
      bool BGVuCVZDhA = false;
      bool eYYkEGnOAb = false;
      bool YFKxRFpFtL = false;
      bool KIWGKpQPZx = false;
      bool pSngAkEypn = false;
      bool XcsLXuuYcw = false;
      bool fTJVpWBSZO = false;
      string riTZDdAhxM;
      string xFuGBxDyrb;
      string VCLNXbPmjK;
      string OPCYAzwMXj;
      string IClJuMNhrg;
      string golBppXFCb;
      string NNxlygCWFC;
      string AqZISTPBSK;
      string NhuDcYdAiJ;
      string aqKwVuFzLN;
      string JqbogoOdiW;
      string ZVlqOfSsJY;
      string DCmwaIgdlX;
      string NQpEBVFFWm;
      string KzRLiEFThp;
      string bLpmKHEfOX;
      string IkqKIJfgor;
      string aNWVNDjaMV;
      string DNuwwCZCtt;
      string RYpyeZIHDZ;
      if(riTZDdAhxM == JqbogoOdiW){YXdoPNoTYG = true;}
      else if(JqbogoOdiW == riTZDdAhxM){iYAoXCBLCL = true;}
      if(xFuGBxDyrb == ZVlqOfSsJY){WcTsTuotOG = true;}
      else if(ZVlqOfSsJY == xFuGBxDyrb){wmLcSPWWhe = true;}
      if(VCLNXbPmjK == DCmwaIgdlX){jcltaBXXpn = true;}
      else if(DCmwaIgdlX == VCLNXbPmjK){JciuuLqsnw = true;}
      if(OPCYAzwMXj == NQpEBVFFWm){ucXmweiGgh = true;}
      else if(NQpEBVFFWm == OPCYAzwMXj){BGVuCVZDhA = true;}
      if(IClJuMNhrg == KzRLiEFThp){NFdnWnFxdj = true;}
      else if(KzRLiEFThp == IClJuMNhrg){eYYkEGnOAb = true;}
      if(golBppXFCb == bLpmKHEfOX){kOrwpukiON = true;}
      else if(bLpmKHEfOX == golBppXFCb){YFKxRFpFtL = true;}
      if(NNxlygCWFC == IkqKIJfgor){pQckLtEfwo = true;}
      else if(IkqKIJfgor == NNxlygCWFC){KIWGKpQPZx = true;}
      if(AqZISTPBSK == aNWVNDjaMV){LiMfsdzyXl = true;}
      if(NhuDcYdAiJ == DNuwwCZCtt){uasGDfjbxB = true;}
      if(aqKwVuFzLN == RYpyeZIHDZ){szqSrwcrbb = true;}
      while(aNWVNDjaMV == AqZISTPBSK){pSngAkEypn = true;}
      while(DNuwwCZCtt == DNuwwCZCtt){XcsLXuuYcw = true;}
      while(RYpyeZIHDZ == RYpyeZIHDZ){fTJVpWBSZO = true;}
      if(YXdoPNoTYG == true){YXdoPNoTYG = false;}
      if(WcTsTuotOG == true){WcTsTuotOG = false;}
      if(jcltaBXXpn == true){jcltaBXXpn = false;}
      if(ucXmweiGgh == true){ucXmweiGgh = false;}
      if(NFdnWnFxdj == true){NFdnWnFxdj = false;}
      if(kOrwpukiON == true){kOrwpukiON = false;}
      if(pQckLtEfwo == true){pQckLtEfwo = false;}
      if(LiMfsdzyXl == true){LiMfsdzyXl = false;}
      if(uasGDfjbxB == true){uasGDfjbxB = false;}
      if(szqSrwcrbb == true){szqSrwcrbb = false;}
      if(iYAoXCBLCL == true){iYAoXCBLCL = false;}
      if(wmLcSPWWhe == true){wmLcSPWWhe = false;}
      if(JciuuLqsnw == true){JciuuLqsnw = false;}
      if(BGVuCVZDhA == true){BGVuCVZDhA = false;}
      if(eYYkEGnOAb == true){eYYkEGnOAb = false;}
      if(YFKxRFpFtL == true){YFKxRFpFtL = false;}
      if(KIWGKpQPZx == true){KIWGKpQPZx = false;}
      if(pSngAkEypn == true){pSngAkEypn = false;}
      if(XcsLXuuYcw == true){XcsLXuuYcw = false;}
      if(fTJVpWBSZO == true){fTJVpWBSZO = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZHNMMKRZTE
{ 
  void bsBjVKKmCJ()
  { 
      bool aWnTRBZwoF = false;
      bool wYGKSRJGxO = false;
      bool eqdmpYwPEe = false;
      bool hwnckAnnoU = false;
      bool cNKwJWwYXL = false;
      bool zoEXqjrAcs = false;
      bool EJqJWAjcYS = false;
      bool dVRblQfhdT = false;
      bool cXqsfsrPkB = false;
      bool InLuPTVBYJ = false;
      bool lSzFhcWbla = false;
      bool DJWJPgRycd = false;
      bool HUrXJoKVLS = false;
      bool teJTgKXlKC = false;
      bool VPdKxyRCfi = false;
      bool BXYuBRZbus = false;
      bool SBQTfYACbF = false;
      bool JfnZHrpgZK = false;
      bool XIpcQWFNRO = false;
      bool jwpQjUzKdX = false;
      string UhVKCpkyRJ;
      string PMyDzykxEC;
      string MJoqwsljLc;
      string lGxLCfBtwe;
      string ZIdRgeRPWN;
      string xfkCFLsdOJ;
      string EBmnWIOlAS;
      string xqAYcEfXKR;
      string bVMyfGUuFq;
      string mWfmZXVotX;
      string PfODQGeLYB;
      string fUTwhhonIQ;
      string mTfJfZLNyF;
      string ipdlCuHIgY;
      string aIVEKSDiFr;
      string hgfdjuzEXp;
      string uRMTRoqgtl;
      string agHiodyayM;
      string WcHGWCKzCG;
      string uCjqdJuBmV;
      if(UhVKCpkyRJ == PfODQGeLYB){aWnTRBZwoF = true;}
      else if(PfODQGeLYB == UhVKCpkyRJ){lSzFhcWbla = true;}
      if(PMyDzykxEC == fUTwhhonIQ){wYGKSRJGxO = true;}
      else if(fUTwhhonIQ == PMyDzykxEC){DJWJPgRycd = true;}
      if(MJoqwsljLc == mTfJfZLNyF){eqdmpYwPEe = true;}
      else if(mTfJfZLNyF == MJoqwsljLc){HUrXJoKVLS = true;}
      if(lGxLCfBtwe == ipdlCuHIgY){hwnckAnnoU = true;}
      else if(ipdlCuHIgY == lGxLCfBtwe){teJTgKXlKC = true;}
      if(ZIdRgeRPWN == aIVEKSDiFr){cNKwJWwYXL = true;}
      else if(aIVEKSDiFr == ZIdRgeRPWN){VPdKxyRCfi = true;}
      if(xfkCFLsdOJ == hgfdjuzEXp){zoEXqjrAcs = true;}
      else if(hgfdjuzEXp == xfkCFLsdOJ){BXYuBRZbus = true;}
      if(EBmnWIOlAS == uRMTRoqgtl){EJqJWAjcYS = true;}
      else if(uRMTRoqgtl == EBmnWIOlAS){SBQTfYACbF = true;}
      if(xqAYcEfXKR == agHiodyayM){dVRblQfhdT = true;}
      if(bVMyfGUuFq == WcHGWCKzCG){cXqsfsrPkB = true;}
      if(mWfmZXVotX == uCjqdJuBmV){InLuPTVBYJ = true;}
      while(agHiodyayM == xqAYcEfXKR){JfnZHrpgZK = true;}
      while(WcHGWCKzCG == WcHGWCKzCG){XIpcQWFNRO = true;}
      while(uCjqdJuBmV == uCjqdJuBmV){jwpQjUzKdX = true;}
      if(aWnTRBZwoF == true){aWnTRBZwoF = false;}
      if(wYGKSRJGxO == true){wYGKSRJGxO = false;}
      if(eqdmpYwPEe == true){eqdmpYwPEe = false;}
      if(hwnckAnnoU == true){hwnckAnnoU = false;}
      if(cNKwJWwYXL == true){cNKwJWwYXL = false;}
      if(zoEXqjrAcs == true){zoEXqjrAcs = false;}
      if(EJqJWAjcYS == true){EJqJWAjcYS = false;}
      if(dVRblQfhdT == true){dVRblQfhdT = false;}
      if(cXqsfsrPkB == true){cXqsfsrPkB = false;}
      if(InLuPTVBYJ == true){InLuPTVBYJ = false;}
      if(lSzFhcWbla == true){lSzFhcWbla = false;}
      if(DJWJPgRycd == true){DJWJPgRycd = false;}
      if(HUrXJoKVLS == true){HUrXJoKVLS = false;}
      if(teJTgKXlKC == true){teJTgKXlKC = false;}
      if(VPdKxyRCfi == true){VPdKxyRCfi = false;}
      if(BXYuBRZbus == true){BXYuBRZbus = false;}
      if(SBQTfYACbF == true){SBQTfYACbF = false;}
      if(JfnZHrpgZK == true){JfnZHrpgZK = false;}
      if(XIpcQWFNRO == true){XIpcQWFNRO = false;}
      if(jwpQjUzKdX == true){jwpQjUzKdX = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JXZWIJOTGT
{ 
  void MpkiDYKdrD()
  { 
      bool EhJOzWWQcw = false;
      bool BRAyNdfCnA = false;
      bool imWNZkFStm = false;
      bool RBTgGTBHiR = false;
      bool QxspbdfOVc = false;
      bool fPRQpRKwnN = false;
      bool REWodzxTqt = false;
      bool bXSViksflw = false;
      bool EpwQHCjDpo = false;
      bool IFSUAMZELc = false;
      bool fMXbAHwugF = false;
      bool XeKrJaHGhb = false;
      bool OtcBkWEyVW = false;
      bool KoTIQqSUPz = false;
      bool FnVkQduaKZ = false;
      bool ETViNkiVpd = false;
      bool CyZKthKBbR = false;
      bool HmpiWZXrjH = false;
      bool admuRpTjue = false;
      bool IxdkwHOLph = false;
      string hftlqdtrne;
      string bqFJhukVkl;
      string lHafVhtwer;
      string qAIHcfitMJ;
      string crKgsiqSmV;
      string sSMQyaFOPJ;
      string jyNwnaiygC;
      string MqJaJtwtlm;
      string pbIJyXnqIC;
      string aTdDZeeOsS;
      string ArlkICIkHD;
      string eLccSHoDxP;
      string lqpdasMGAd;
      string PgASlyoWAw;
      string OCiFXPqBjR;
      string oaRdfICHJP;
      string oCkcnxKJqU;
      string SSDAAjHjGL;
      string BLWhonOWmc;
      string ugMjGMRFQZ;
      if(hftlqdtrne == ArlkICIkHD){EhJOzWWQcw = true;}
      else if(ArlkICIkHD == hftlqdtrne){fMXbAHwugF = true;}
      if(bqFJhukVkl == eLccSHoDxP){BRAyNdfCnA = true;}
      else if(eLccSHoDxP == bqFJhukVkl){XeKrJaHGhb = true;}
      if(lHafVhtwer == lqpdasMGAd){imWNZkFStm = true;}
      else if(lqpdasMGAd == lHafVhtwer){OtcBkWEyVW = true;}
      if(qAIHcfitMJ == PgASlyoWAw){RBTgGTBHiR = true;}
      else if(PgASlyoWAw == qAIHcfitMJ){KoTIQqSUPz = true;}
      if(crKgsiqSmV == OCiFXPqBjR){QxspbdfOVc = true;}
      else if(OCiFXPqBjR == crKgsiqSmV){FnVkQduaKZ = true;}
      if(sSMQyaFOPJ == oaRdfICHJP){fPRQpRKwnN = true;}
      else if(oaRdfICHJP == sSMQyaFOPJ){ETViNkiVpd = true;}
      if(jyNwnaiygC == oCkcnxKJqU){REWodzxTqt = true;}
      else if(oCkcnxKJqU == jyNwnaiygC){CyZKthKBbR = true;}
      if(MqJaJtwtlm == SSDAAjHjGL){bXSViksflw = true;}
      if(pbIJyXnqIC == BLWhonOWmc){EpwQHCjDpo = true;}
      if(aTdDZeeOsS == ugMjGMRFQZ){IFSUAMZELc = true;}
      while(SSDAAjHjGL == MqJaJtwtlm){HmpiWZXrjH = true;}
      while(BLWhonOWmc == BLWhonOWmc){admuRpTjue = true;}
      while(ugMjGMRFQZ == ugMjGMRFQZ){IxdkwHOLph = true;}
      if(EhJOzWWQcw == true){EhJOzWWQcw = false;}
      if(BRAyNdfCnA == true){BRAyNdfCnA = false;}
      if(imWNZkFStm == true){imWNZkFStm = false;}
      if(RBTgGTBHiR == true){RBTgGTBHiR = false;}
      if(QxspbdfOVc == true){QxspbdfOVc = false;}
      if(fPRQpRKwnN == true){fPRQpRKwnN = false;}
      if(REWodzxTqt == true){REWodzxTqt = false;}
      if(bXSViksflw == true){bXSViksflw = false;}
      if(EpwQHCjDpo == true){EpwQHCjDpo = false;}
      if(IFSUAMZELc == true){IFSUAMZELc = false;}
      if(fMXbAHwugF == true){fMXbAHwugF = false;}
      if(XeKrJaHGhb == true){XeKrJaHGhb = false;}
      if(OtcBkWEyVW == true){OtcBkWEyVW = false;}
      if(KoTIQqSUPz == true){KoTIQqSUPz = false;}
      if(FnVkQduaKZ == true){FnVkQduaKZ = false;}
      if(ETViNkiVpd == true){ETViNkiVpd = false;}
      if(CyZKthKBbR == true){CyZKthKBbR = false;}
      if(HmpiWZXrjH == true){HmpiWZXrjH = false;}
      if(admuRpTjue == true){admuRpTjue = false;}
      if(IxdkwHOLph == true){IxdkwHOLph = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CZMUCWINNJ
{ 
  void UtDxSmDxBy()
  { 
      bool GLHPBZQUAV = false;
      bool CsSuRfzohg = false;
      bool hMcZJBxyfq = false;
      bool SJUzfPifRy = false;
      bool GgxlOxaSBV = false;
      bool KthlGshgGz = false;
      bool bSoIDrXSPC = false;
      bool wSXjaBPpKL = false;
      bool aewJgXaisl = false;
      bool gCZRWmqniW = false;
      bool ZHCyZqVyLE = false;
      bool dqVtXwVebe = false;
      bool QnZdVsrFNm = false;
      bool JaHnlTbyjo = false;
      bool BcXnHpffsZ = false;
      bool QMMEFkgYcf = false;
      bool zPkXuDjhHE = false;
      bool TXulcmPlVe = false;
      bool NMMmVwOCiB = false;
      bool lCUDpkrAsD = false;
      string PDtosStyKi;
      string yuLocWOXFq;
      string wMjbiEHClT;
      string QoIQQwhpZA;
      string mZzabNnqSG;
      string jEaUgCTRGn;
      string KrLpSJltYg;
      string oXxEEVNIzV;
      string QQifjLOmiN;
      string bPilptOIIS;
      string asRlRLGWQL;
      string oKSkWtIfFG;
      string tyLLEYoLOJ;
      string GEAgtXzwNX;
      string FrWXYRxyOw;
      string liCBXcgFhs;
      string WuHGEjKlqz;
      string uLRhBCanml;
      string aqwSXXByIQ;
      string psOTBJjQSn;
      if(PDtosStyKi == asRlRLGWQL){GLHPBZQUAV = true;}
      else if(asRlRLGWQL == PDtosStyKi){ZHCyZqVyLE = true;}
      if(yuLocWOXFq == oKSkWtIfFG){CsSuRfzohg = true;}
      else if(oKSkWtIfFG == yuLocWOXFq){dqVtXwVebe = true;}
      if(wMjbiEHClT == tyLLEYoLOJ){hMcZJBxyfq = true;}
      else if(tyLLEYoLOJ == wMjbiEHClT){QnZdVsrFNm = true;}
      if(QoIQQwhpZA == GEAgtXzwNX){SJUzfPifRy = true;}
      else if(GEAgtXzwNX == QoIQQwhpZA){JaHnlTbyjo = true;}
      if(mZzabNnqSG == FrWXYRxyOw){GgxlOxaSBV = true;}
      else if(FrWXYRxyOw == mZzabNnqSG){BcXnHpffsZ = true;}
      if(jEaUgCTRGn == liCBXcgFhs){KthlGshgGz = true;}
      else if(liCBXcgFhs == jEaUgCTRGn){QMMEFkgYcf = true;}
      if(KrLpSJltYg == WuHGEjKlqz){bSoIDrXSPC = true;}
      else if(WuHGEjKlqz == KrLpSJltYg){zPkXuDjhHE = true;}
      if(oXxEEVNIzV == uLRhBCanml){wSXjaBPpKL = true;}
      if(QQifjLOmiN == aqwSXXByIQ){aewJgXaisl = true;}
      if(bPilptOIIS == psOTBJjQSn){gCZRWmqniW = true;}
      while(uLRhBCanml == oXxEEVNIzV){TXulcmPlVe = true;}
      while(aqwSXXByIQ == aqwSXXByIQ){NMMmVwOCiB = true;}
      while(psOTBJjQSn == psOTBJjQSn){lCUDpkrAsD = true;}
      if(GLHPBZQUAV == true){GLHPBZQUAV = false;}
      if(CsSuRfzohg == true){CsSuRfzohg = false;}
      if(hMcZJBxyfq == true){hMcZJBxyfq = false;}
      if(SJUzfPifRy == true){SJUzfPifRy = false;}
      if(GgxlOxaSBV == true){GgxlOxaSBV = false;}
      if(KthlGshgGz == true){KthlGshgGz = false;}
      if(bSoIDrXSPC == true){bSoIDrXSPC = false;}
      if(wSXjaBPpKL == true){wSXjaBPpKL = false;}
      if(aewJgXaisl == true){aewJgXaisl = false;}
      if(gCZRWmqniW == true){gCZRWmqniW = false;}
      if(ZHCyZqVyLE == true){ZHCyZqVyLE = false;}
      if(dqVtXwVebe == true){dqVtXwVebe = false;}
      if(QnZdVsrFNm == true){QnZdVsrFNm = false;}
      if(JaHnlTbyjo == true){JaHnlTbyjo = false;}
      if(BcXnHpffsZ == true){BcXnHpffsZ = false;}
      if(QMMEFkgYcf == true){QMMEFkgYcf = false;}
      if(zPkXuDjhHE == true){zPkXuDjhHE = false;}
      if(TXulcmPlVe == true){TXulcmPlVe = false;}
      if(NMMmVwOCiB == true){NMMmVwOCiB = false;}
      if(lCUDpkrAsD == true){lCUDpkrAsD = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NGZTFWBPYP
{ 
  void hQqukBAmVN()
  { 
      bool aagAWjBWaj = false;
      bool qCnDibPGrk = false;
      bool riTzfiIRma = false;
      bool hTretBJYkW = false;
      bool SJXdFekpaw = false;
      bool cMWQdqGgTn = false;
      bool bjiABWJJJh = false;
      bool SpVURhLpsB = false;
      bool xRpqxVxbBC = false;
      bool tbExuiaXci = false;
      bool UbgrclqByu = false;
      bool fQmSpJsrCe = false;
      bool ZhIGmpkYZi = false;
      bool HnMKfVkyNL = false;
      bool ewcFWdolGk = false;
      bool ohUxImSaEl = false;
      bool ASaPmrDbmT = false;
      bool YudqAcECiJ = false;
      bool BzRntZSPBs = false;
      bool CJsaKMUNRe = false;
      string NAVIHxeJUp;
      string IVAJEbsVbH;
      string RWoPklVksm;
      string fkLnbrxXbg;
      string CrTDskJTOa;
      string IWyAdqKFWU;
      string bsQhJWKLgL;
      string dVttEZClWA;
      string bCWSdJlLKj;
      string hrBGFJbltV;
      string axlDwmbEuD;
      string oYiWrYVxDA;
      string jzBwdkUmou;
      string yPrAYdPTVq;
      string ghPGuIJdNf;
      string mdislawpYL;
      string SoOAFYgixN;
      string JCbLSBitBV;
      string bpYIHUgyOx;
      string hZtKYrSOBV;
      if(NAVIHxeJUp == axlDwmbEuD){aagAWjBWaj = true;}
      else if(axlDwmbEuD == NAVIHxeJUp){UbgrclqByu = true;}
      if(IVAJEbsVbH == oYiWrYVxDA){qCnDibPGrk = true;}
      else if(oYiWrYVxDA == IVAJEbsVbH){fQmSpJsrCe = true;}
      if(RWoPklVksm == jzBwdkUmou){riTzfiIRma = true;}
      else if(jzBwdkUmou == RWoPklVksm){ZhIGmpkYZi = true;}
      if(fkLnbrxXbg == yPrAYdPTVq){hTretBJYkW = true;}
      else if(yPrAYdPTVq == fkLnbrxXbg){HnMKfVkyNL = true;}
      if(CrTDskJTOa == ghPGuIJdNf){SJXdFekpaw = true;}
      else if(ghPGuIJdNf == CrTDskJTOa){ewcFWdolGk = true;}
      if(IWyAdqKFWU == mdislawpYL){cMWQdqGgTn = true;}
      else if(mdislawpYL == IWyAdqKFWU){ohUxImSaEl = true;}
      if(bsQhJWKLgL == SoOAFYgixN){bjiABWJJJh = true;}
      else if(SoOAFYgixN == bsQhJWKLgL){ASaPmrDbmT = true;}
      if(dVttEZClWA == JCbLSBitBV){SpVURhLpsB = true;}
      if(bCWSdJlLKj == bpYIHUgyOx){xRpqxVxbBC = true;}
      if(hrBGFJbltV == hZtKYrSOBV){tbExuiaXci = true;}
      while(JCbLSBitBV == dVttEZClWA){YudqAcECiJ = true;}
      while(bpYIHUgyOx == bpYIHUgyOx){BzRntZSPBs = true;}
      while(hZtKYrSOBV == hZtKYrSOBV){CJsaKMUNRe = true;}
      if(aagAWjBWaj == true){aagAWjBWaj = false;}
      if(qCnDibPGrk == true){qCnDibPGrk = false;}
      if(riTzfiIRma == true){riTzfiIRma = false;}
      if(hTretBJYkW == true){hTretBJYkW = false;}
      if(SJXdFekpaw == true){SJXdFekpaw = false;}
      if(cMWQdqGgTn == true){cMWQdqGgTn = false;}
      if(bjiABWJJJh == true){bjiABWJJJh = false;}
      if(SpVURhLpsB == true){SpVURhLpsB = false;}
      if(xRpqxVxbBC == true){xRpqxVxbBC = false;}
      if(tbExuiaXci == true){tbExuiaXci = false;}
      if(UbgrclqByu == true){UbgrclqByu = false;}
      if(fQmSpJsrCe == true){fQmSpJsrCe = false;}
      if(ZhIGmpkYZi == true){ZhIGmpkYZi = false;}
      if(HnMKfVkyNL == true){HnMKfVkyNL = false;}
      if(ewcFWdolGk == true){ewcFWdolGk = false;}
      if(ohUxImSaEl == true){ohUxImSaEl = false;}
      if(ASaPmrDbmT == true){ASaPmrDbmT = false;}
      if(YudqAcECiJ == true){YudqAcECiJ = false;}
      if(BzRntZSPBs == true){BzRntZSPBs = false;}
      if(CJsaKMUNRe == true){CJsaKMUNRe = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UDTIQUPTTV
{ 
  void WokQkZehOM()
  { 
      bool gootzjBpSZ = false;
      bool OJFHMVNgAU = false;
      bool WNjPmXfkdn = false;
      bool tPGOHNOQBd = false;
      bool FfNkhGcgtb = false;
      bool MfbPmSFYGb = false;
      bool DPLefzCpzy = false;
      bool GxfWPpAzhg = false;
      bool tyqBaOmpbo = false;
      bool IoKZZoTiFi = false;
      bool DlguVPzFzw = false;
      bool RGqZgFXKDQ = false;
      bool MLOJpQmGwa = false;
      bool WRszcHQwfL = false;
      bool PgqeXByjrU = false;
      bool QJeoVrXXFE = false;
      bool csJYZzMrwZ = false;
      bool nbdrBeUiln = false;
      bool SCObJNlEck = false;
      bool isqbiVXSFX = false;
      string pWXJGcGhgp;
      string pmRxAKNhqk;
      string CtEDuRoGtV;
      string rDeYhBujGM;
      string JuhtXzxVKT;
      string MJTHiuSUOn;
      string mAnLxmHsOy;
      string PdbeaHMKew;
      string UTwZIhjXol;
      string iaVRYIGVwk;
      string qEGLEihnfb;
      string iHoRTTENCO;
      string DjDbPoHJGR;
      string ApNsEQMjVZ;
      string AuqMDakDyF;
      string EsTghHAduP;
      string aYhObstuND;
      string GUqcOHrqYn;
      string DEKANGWfLx;
      string PinGyaMckJ;
      if(pWXJGcGhgp == qEGLEihnfb){gootzjBpSZ = true;}
      else if(qEGLEihnfb == pWXJGcGhgp){DlguVPzFzw = true;}
      if(pmRxAKNhqk == iHoRTTENCO){OJFHMVNgAU = true;}
      else if(iHoRTTENCO == pmRxAKNhqk){RGqZgFXKDQ = true;}
      if(CtEDuRoGtV == DjDbPoHJGR){WNjPmXfkdn = true;}
      else if(DjDbPoHJGR == CtEDuRoGtV){MLOJpQmGwa = true;}
      if(rDeYhBujGM == ApNsEQMjVZ){tPGOHNOQBd = true;}
      else if(ApNsEQMjVZ == rDeYhBujGM){WRszcHQwfL = true;}
      if(JuhtXzxVKT == AuqMDakDyF){FfNkhGcgtb = true;}
      else if(AuqMDakDyF == JuhtXzxVKT){PgqeXByjrU = true;}
      if(MJTHiuSUOn == EsTghHAduP){MfbPmSFYGb = true;}
      else if(EsTghHAduP == MJTHiuSUOn){QJeoVrXXFE = true;}
      if(mAnLxmHsOy == aYhObstuND){DPLefzCpzy = true;}
      else if(aYhObstuND == mAnLxmHsOy){csJYZzMrwZ = true;}
      if(PdbeaHMKew == GUqcOHrqYn){GxfWPpAzhg = true;}
      if(UTwZIhjXol == DEKANGWfLx){tyqBaOmpbo = true;}
      if(iaVRYIGVwk == PinGyaMckJ){IoKZZoTiFi = true;}
      while(GUqcOHrqYn == PdbeaHMKew){nbdrBeUiln = true;}
      while(DEKANGWfLx == DEKANGWfLx){SCObJNlEck = true;}
      while(PinGyaMckJ == PinGyaMckJ){isqbiVXSFX = true;}
      if(gootzjBpSZ == true){gootzjBpSZ = false;}
      if(OJFHMVNgAU == true){OJFHMVNgAU = false;}
      if(WNjPmXfkdn == true){WNjPmXfkdn = false;}
      if(tPGOHNOQBd == true){tPGOHNOQBd = false;}
      if(FfNkhGcgtb == true){FfNkhGcgtb = false;}
      if(MfbPmSFYGb == true){MfbPmSFYGb = false;}
      if(DPLefzCpzy == true){DPLefzCpzy = false;}
      if(GxfWPpAzhg == true){GxfWPpAzhg = false;}
      if(tyqBaOmpbo == true){tyqBaOmpbo = false;}
      if(IoKZZoTiFi == true){IoKZZoTiFi = false;}
      if(DlguVPzFzw == true){DlguVPzFzw = false;}
      if(RGqZgFXKDQ == true){RGqZgFXKDQ = false;}
      if(MLOJpQmGwa == true){MLOJpQmGwa = false;}
      if(WRszcHQwfL == true){WRszcHQwfL = false;}
      if(PgqeXByjrU == true){PgqeXByjrU = false;}
      if(QJeoVrXXFE == true){QJeoVrXXFE = false;}
      if(csJYZzMrwZ == true){csJYZzMrwZ = false;}
      if(nbdrBeUiln == true){nbdrBeUiln = false;}
      if(SCObJNlEck == true){SCObJNlEck = false;}
      if(isqbiVXSFX == true){isqbiVXSFX = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UXXGTYXKJH
{ 
  void GoSytqEkFh()
  { 
      bool nkDOxGWElY = false;
      bool rKcSMiyEei = false;
      bool KgGcPoMCDU = false;
      bool RuADylmxHA = false;
      bool CCuWYPqWtQ = false;
      bool uIIQLGdayI = false;
      bool tUARHxJZUZ = false;
      bool uAxdgHkjDz = false;
      bool ypImYJljqq = false;
      bool IMyzGgCdPb = false;
      bool GRBTUuKdjD = false;
      bool UsWyJEMMsx = false;
      bool KZhcRhBxXz = false;
      bool FHUbsgnnRc = false;
      bool yIznWOpAoC = false;
      bool UokRQgiYBm = false;
      bool sNGkyNYuFc = false;
      bool PXuPyCZtGD = false;
      bool rBIUePWCLL = false;
      bool xOlejkCOoI = false;
      string oVHVmMNWAK;
      string ImBqnxiRJO;
      string GFWBCEqZih;
      string KOAWFzwGRI;
      string sTETxWyCjw;
      string rcWXAizGTi;
      string UmmSAsleko;
      string BcAAVlBCDe;
      string FnplogrcVW;
      string zpmhnFgkTE;
      string kTRtuPcVCN;
      string hxCncgxAcS;
      string AJIOoLoRMP;
      string FPhzslWuqZ;
      string EVZHnCmyFH;
      string IfIADbLCFj;
      string ZKJlIROCuq;
      string JCIDOdiAZl;
      string XqMFHYHhuR;
      string aToGztMnzW;
      if(oVHVmMNWAK == kTRtuPcVCN){nkDOxGWElY = true;}
      else if(kTRtuPcVCN == oVHVmMNWAK){GRBTUuKdjD = true;}
      if(ImBqnxiRJO == hxCncgxAcS){rKcSMiyEei = true;}
      else if(hxCncgxAcS == ImBqnxiRJO){UsWyJEMMsx = true;}
      if(GFWBCEqZih == AJIOoLoRMP){KgGcPoMCDU = true;}
      else if(AJIOoLoRMP == GFWBCEqZih){KZhcRhBxXz = true;}
      if(KOAWFzwGRI == FPhzslWuqZ){RuADylmxHA = true;}
      else if(FPhzslWuqZ == KOAWFzwGRI){FHUbsgnnRc = true;}
      if(sTETxWyCjw == EVZHnCmyFH){CCuWYPqWtQ = true;}
      else if(EVZHnCmyFH == sTETxWyCjw){yIznWOpAoC = true;}
      if(rcWXAizGTi == IfIADbLCFj){uIIQLGdayI = true;}
      else if(IfIADbLCFj == rcWXAizGTi){UokRQgiYBm = true;}
      if(UmmSAsleko == ZKJlIROCuq){tUARHxJZUZ = true;}
      else if(ZKJlIROCuq == UmmSAsleko){sNGkyNYuFc = true;}
      if(BcAAVlBCDe == JCIDOdiAZl){uAxdgHkjDz = true;}
      if(FnplogrcVW == XqMFHYHhuR){ypImYJljqq = true;}
      if(zpmhnFgkTE == aToGztMnzW){IMyzGgCdPb = true;}
      while(JCIDOdiAZl == BcAAVlBCDe){PXuPyCZtGD = true;}
      while(XqMFHYHhuR == XqMFHYHhuR){rBIUePWCLL = true;}
      while(aToGztMnzW == aToGztMnzW){xOlejkCOoI = true;}
      if(nkDOxGWElY == true){nkDOxGWElY = false;}
      if(rKcSMiyEei == true){rKcSMiyEei = false;}
      if(KgGcPoMCDU == true){KgGcPoMCDU = false;}
      if(RuADylmxHA == true){RuADylmxHA = false;}
      if(CCuWYPqWtQ == true){CCuWYPqWtQ = false;}
      if(uIIQLGdayI == true){uIIQLGdayI = false;}
      if(tUARHxJZUZ == true){tUARHxJZUZ = false;}
      if(uAxdgHkjDz == true){uAxdgHkjDz = false;}
      if(ypImYJljqq == true){ypImYJljqq = false;}
      if(IMyzGgCdPb == true){IMyzGgCdPb = false;}
      if(GRBTUuKdjD == true){GRBTUuKdjD = false;}
      if(UsWyJEMMsx == true){UsWyJEMMsx = false;}
      if(KZhcRhBxXz == true){KZhcRhBxXz = false;}
      if(FHUbsgnnRc == true){FHUbsgnnRc = false;}
      if(yIznWOpAoC == true){yIznWOpAoC = false;}
      if(UokRQgiYBm == true){UokRQgiYBm = false;}
      if(sNGkyNYuFc == true){sNGkyNYuFc = false;}
      if(PXuPyCZtGD == true){PXuPyCZtGD = false;}
      if(rBIUePWCLL == true){rBIUePWCLL = false;}
      if(xOlejkCOoI == true){xOlejkCOoI = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class WSOHKUWZJT
{ 
  void EpSwdulwoL()
  { 
      bool byPUxRPFEX = false;
      bool lrWyMztGLt = false;
      bool BPCinaYbqE = false;
      bool tFkaFanNNt = false;
      bool FlcgxQOpri = false;
      bool GBzJtMwggW = false;
      bool rmOzcgHwrk = false;
      bool KgrQoOlUkX = false;
      bool LijHysGbgs = false;
      bool eHgqBRWcDQ = false;
      bool JSxGYYIhUe = false;
      bool ZbuJKJDGBj = false;
      bool afOyGDABOW = false;
      bool qJRUNskciY = false;
      bool jXGCSCLXfr = false;
      bool OQbTpdDyAU = false;
      bool YGSxmQJauQ = false;
      bool XZXczDYMfD = false;
      bool qcUWIagyej = false;
      bool plcNAcEJOC = false;
      string fDyYJWMkko;
      string IGaCOnJoqI;
      string nyabfjzUCw;
      string uTMRmqPgfW;
      string EDhnrsASLr;
      string SmGRRhbVrY;
      string WqiososJPW;
      string YbITDkQmim;
      string igJKcIMuzF;
      string OsnHzMUruD;
      string AqCnSqdlNf;
      string QomFophXMj;
      string cxJKHfmAUp;
      string QXrflqdUql;
      string eZgTnZFiSJ;
      string ewfKaChrfm;
      string DcRWPsxHSx;
      string LNgZFCPoVh;
      string ZEhLLUrgFj;
      string JknEiBHKEu;
      if(fDyYJWMkko == AqCnSqdlNf){byPUxRPFEX = true;}
      else if(AqCnSqdlNf == fDyYJWMkko){JSxGYYIhUe = true;}
      if(IGaCOnJoqI == QomFophXMj){lrWyMztGLt = true;}
      else if(QomFophXMj == IGaCOnJoqI){ZbuJKJDGBj = true;}
      if(nyabfjzUCw == cxJKHfmAUp){BPCinaYbqE = true;}
      else if(cxJKHfmAUp == nyabfjzUCw){afOyGDABOW = true;}
      if(uTMRmqPgfW == QXrflqdUql){tFkaFanNNt = true;}
      else if(QXrflqdUql == uTMRmqPgfW){qJRUNskciY = true;}
      if(EDhnrsASLr == eZgTnZFiSJ){FlcgxQOpri = true;}
      else if(eZgTnZFiSJ == EDhnrsASLr){jXGCSCLXfr = true;}
      if(SmGRRhbVrY == ewfKaChrfm){GBzJtMwggW = true;}
      else if(ewfKaChrfm == SmGRRhbVrY){OQbTpdDyAU = true;}
      if(WqiososJPW == DcRWPsxHSx){rmOzcgHwrk = true;}
      else if(DcRWPsxHSx == WqiososJPW){YGSxmQJauQ = true;}
      if(YbITDkQmim == LNgZFCPoVh){KgrQoOlUkX = true;}
      if(igJKcIMuzF == ZEhLLUrgFj){LijHysGbgs = true;}
      if(OsnHzMUruD == JknEiBHKEu){eHgqBRWcDQ = true;}
      while(LNgZFCPoVh == YbITDkQmim){XZXczDYMfD = true;}
      while(ZEhLLUrgFj == ZEhLLUrgFj){qcUWIagyej = true;}
      while(JknEiBHKEu == JknEiBHKEu){plcNAcEJOC = true;}
      if(byPUxRPFEX == true){byPUxRPFEX = false;}
      if(lrWyMztGLt == true){lrWyMztGLt = false;}
      if(BPCinaYbqE == true){BPCinaYbqE = false;}
      if(tFkaFanNNt == true){tFkaFanNNt = false;}
      if(FlcgxQOpri == true){FlcgxQOpri = false;}
      if(GBzJtMwggW == true){GBzJtMwggW = false;}
      if(rmOzcgHwrk == true){rmOzcgHwrk = false;}
      if(KgrQoOlUkX == true){KgrQoOlUkX = false;}
      if(LijHysGbgs == true){LijHysGbgs = false;}
      if(eHgqBRWcDQ == true){eHgqBRWcDQ = false;}
      if(JSxGYYIhUe == true){JSxGYYIhUe = false;}
      if(ZbuJKJDGBj == true){ZbuJKJDGBj = false;}
      if(afOyGDABOW == true){afOyGDABOW = false;}
      if(qJRUNskciY == true){qJRUNskciY = false;}
      if(jXGCSCLXfr == true){jXGCSCLXfr = false;}
      if(OQbTpdDyAU == true){OQbTpdDyAU = false;}
      if(YGSxmQJauQ == true){YGSxmQJauQ = false;}
      if(XZXczDYMfD == true){XZXczDYMfD = false;}
      if(qcUWIagyej == true){qcUWIagyej = false;}
      if(plcNAcEJOC == true){plcNAcEJOC = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NUSCNONJNG
{ 
  void eBDEKzRsMy()
  { 
      bool tpFbPBrbdw = false;
      bool pejeueyKDZ = false;
      bool sQgLpdAony = false;
      bool KYwkosQlPZ = false;
      bool WteeYzxiOu = false;
      bool gSpobdJtlk = false;
      bool BCnfkQiypC = false;
      bool OMtcCzlBmq = false;
      bool gQZsRriGJL = false;
      bool jWzdYTFytH = false;
      bool PysMfTXMlb = false;
      bool qHGtEjcuOs = false;
      bool hboeRrMPDh = false;
      bool smHfaiWNMx = false;
      bool TVTgTbPFZn = false;
      bool qKjAYmIOum = false;
      bool HHIDhEAaTU = false;
      bool NOhRYLIHKp = false;
      bool cHpPpiCGGm = false;
      bool aIRYoMMQzP = false;
      string cHmZWwdSwe;
      string ELTwTshEaH;
      string aDRPGEBkWW;
      string phYxnCpyEG;
      string DgHUNkcrlR;
      string nXRcsYoWTR;
      string trwZoWlVHR;
      string CizTEsuxCV;
      string bJyFhRQwEO;
      string MTemMKjhfp;
      string yHjTOFYxXz;
      string drUyoktqqW;
      string GpQrEiDLHs;
      string JNRLWUYdpc;
      string sSXFkUzgxb;
      string QsqIjIpqPe;
      string BMpepPZIaU;
      string LPAOdmRLGj;
      string jWOfbPWJbO;
      string dgOKafcsPV;
      if(cHmZWwdSwe == yHjTOFYxXz){tpFbPBrbdw = true;}
      else if(yHjTOFYxXz == cHmZWwdSwe){PysMfTXMlb = true;}
      if(ELTwTshEaH == drUyoktqqW){pejeueyKDZ = true;}
      else if(drUyoktqqW == ELTwTshEaH){qHGtEjcuOs = true;}
      if(aDRPGEBkWW == GpQrEiDLHs){sQgLpdAony = true;}
      else if(GpQrEiDLHs == aDRPGEBkWW){hboeRrMPDh = true;}
      if(phYxnCpyEG == JNRLWUYdpc){KYwkosQlPZ = true;}
      else if(JNRLWUYdpc == phYxnCpyEG){smHfaiWNMx = true;}
      if(DgHUNkcrlR == sSXFkUzgxb){WteeYzxiOu = true;}
      else if(sSXFkUzgxb == DgHUNkcrlR){TVTgTbPFZn = true;}
      if(nXRcsYoWTR == QsqIjIpqPe){gSpobdJtlk = true;}
      else if(QsqIjIpqPe == nXRcsYoWTR){qKjAYmIOum = true;}
      if(trwZoWlVHR == BMpepPZIaU){BCnfkQiypC = true;}
      else if(BMpepPZIaU == trwZoWlVHR){HHIDhEAaTU = true;}
      if(CizTEsuxCV == LPAOdmRLGj){OMtcCzlBmq = true;}
      if(bJyFhRQwEO == jWOfbPWJbO){gQZsRriGJL = true;}
      if(MTemMKjhfp == dgOKafcsPV){jWzdYTFytH = true;}
      while(LPAOdmRLGj == CizTEsuxCV){NOhRYLIHKp = true;}
      while(jWOfbPWJbO == jWOfbPWJbO){cHpPpiCGGm = true;}
      while(dgOKafcsPV == dgOKafcsPV){aIRYoMMQzP = true;}
      if(tpFbPBrbdw == true){tpFbPBrbdw = false;}
      if(pejeueyKDZ == true){pejeueyKDZ = false;}
      if(sQgLpdAony == true){sQgLpdAony = false;}
      if(KYwkosQlPZ == true){KYwkosQlPZ = false;}
      if(WteeYzxiOu == true){WteeYzxiOu = false;}
      if(gSpobdJtlk == true){gSpobdJtlk = false;}
      if(BCnfkQiypC == true){BCnfkQiypC = false;}
      if(OMtcCzlBmq == true){OMtcCzlBmq = false;}
      if(gQZsRriGJL == true){gQZsRriGJL = false;}
      if(jWzdYTFytH == true){jWzdYTFytH = false;}
      if(PysMfTXMlb == true){PysMfTXMlb = false;}
      if(qHGtEjcuOs == true){qHGtEjcuOs = false;}
      if(hboeRrMPDh == true){hboeRrMPDh = false;}
      if(smHfaiWNMx == true){smHfaiWNMx = false;}
      if(TVTgTbPFZn == true){TVTgTbPFZn = false;}
      if(qKjAYmIOum == true){qKjAYmIOum = false;}
      if(HHIDhEAaTU == true){HHIDhEAaTU = false;}
      if(NOhRYLIHKp == true){NOhRYLIHKp = false;}
      if(cHpPpiCGGm == true){cHpPpiCGGm = false;}
      if(aIRYoMMQzP == true){aIRYoMMQzP = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QPXJSCFVYT
{ 
  void qCVSSpFMnk()
  { 
      bool PWQUQBGQuu = false;
      bool ztAduzHqjj = false;
      bool lPALKisXol = false;
      bool rNVrNyskWp = false;
      bool goLOdpdgKr = false;
      bool TbqYTAVFJh = false;
      bool HhbIClXIQw = false;
      bool FWUsDwDkgT = false;
      bool ELMwSQWwYM = false;
      bool cnanhyMDMr = false;
      bool SUQOmxHxVW = false;
      bool xjYpcwUfpl = false;
      bool OajETRufLF = false;
      bool iAYnAjuyJV = false;
      bool IYaBOrPkLl = false;
      bool cgMZdrkMtU = false;
      bool BalQmxKjos = false;
      bool BWWUcpAhpb = false;
      bool CKUYbCmzKI = false;
      bool ziDnThmJNQ = false;
      string MXiTghVIzS;
      string nzgafoLOWS;
      string mcJRHcQGTm;
      string uecnfobcYk;
      string rarhdEfqPe;
      string JYizJiORMD;
      string QOoLXIVrtq;
      string MhplaLkamC;
      string xZqncVwcex;
      string aKQUUclfKU;
      string imwXRbhwYx;
      string xmdJFpXAhB;
      string MsNxQCRURx;
      string ZEQSTKcSVB;
      string nzDgzmgGPd;
      string FkxIIoZzHP;
      string JMebXsWKpu;
      string MEdgNMofyc;
      string UKFmqnGHKd;
      string jdLGraTbJF;
      if(MXiTghVIzS == imwXRbhwYx){PWQUQBGQuu = true;}
      else if(imwXRbhwYx == MXiTghVIzS){SUQOmxHxVW = true;}
      if(nzgafoLOWS == xmdJFpXAhB){ztAduzHqjj = true;}
      else if(xmdJFpXAhB == nzgafoLOWS){xjYpcwUfpl = true;}
      if(mcJRHcQGTm == MsNxQCRURx){lPALKisXol = true;}
      else if(MsNxQCRURx == mcJRHcQGTm){OajETRufLF = true;}
      if(uecnfobcYk == ZEQSTKcSVB){rNVrNyskWp = true;}
      else if(ZEQSTKcSVB == uecnfobcYk){iAYnAjuyJV = true;}
      if(rarhdEfqPe == nzDgzmgGPd){goLOdpdgKr = true;}
      else if(nzDgzmgGPd == rarhdEfqPe){IYaBOrPkLl = true;}
      if(JYizJiORMD == FkxIIoZzHP){TbqYTAVFJh = true;}
      else if(FkxIIoZzHP == JYizJiORMD){cgMZdrkMtU = true;}
      if(QOoLXIVrtq == JMebXsWKpu){HhbIClXIQw = true;}
      else if(JMebXsWKpu == QOoLXIVrtq){BalQmxKjos = true;}
      if(MhplaLkamC == MEdgNMofyc){FWUsDwDkgT = true;}
      if(xZqncVwcex == UKFmqnGHKd){ELMwSQWwYM = true;}
      if(aKQUUclfKU == jdLGraTbJF){cnanhyMDMr = true;}
      while(MEdgNMofyc == MhplaLkamC){BWWUcpAhpb = true;}
      while(UKFmqnGHKd == UKFmqnGHKd){CKUYbCmzKI = true;}
      while(jdLGraTbJF == jdLGraTbJF){ziDnThmJNQ = true;}
      if(PWQUQBGQuu == true){PWQUQBGQuu = false;}
      if(ztAduzHqjj == true){ztAduzHqjj = false;}
      if(lPALKisXol == true){lPALKisXol = false;}
      if(rNVrNyskWp == true){rNVrNyskWp = false;}
      if(goLOdpdgKr == true){goLOdpdgKr = false;}
      if(TbqYTAVFJh == true){TbqYTAVFJh = false;}
      if(HhbIClXIQw == true){HhbIClXIQw = false;}
      if(FWUsDwDkgT == true){FWUsDwDkgT = false;}
      if(ELMwSQWwYM == true){ELMwSQWwYM = false;}
      if(cnanhyMDMr == true){cnanhyMDMr = false;}
      if(SUQOmxHxVW == true){SUQOmxHxVW = false;}
      if(xjYpcwUfpl == true){xjYpcwUfpl = false;}
      if(OajETRufLF == true){OajETRufLF = false;}
      if(iAYnAjuyJV == true){iAYnAjuyJV = false;}
      if(IYaBOrPkLl == true){IYaBOrPkLl = false;}
      if(cgMZdrkMtU == true){cgMZdrkMtU = false;}
      if(BalQmxKjos == true){BalQmxKjos = false;}
      if(BWWUcpAhpb == true){BWWUcpAhpb = false;}
      if(CKUYbCmzKI == true){CKUYbCmzKI = false;}
      if(ziDnThmJNQ == true){ziDnThmJNQ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class EYGEMNORMX
{ 
  void gKpKfiEyJo()
  { 
      bool JuNrBZGVHF = false;
      bool lfThAlekiY = false;
      bool qRnNWaYsEl = false;
      bool cfCVmEJIVg = false;
      bool NrQQnHcftO = false;
      bool mpRCjrHAUa = false;
      bool BMQVbFcxtw = false;
      bool cVOBLzBWYO = false;
      bool yszpzmNAlu = false;
      bool wInufAatoh = false;
      bool dekxlQaQqj = false;
      bool pkbFbHNaWo = false;
      bool rcLzzThTUK = false;
      bool XGgpkSHBDh = false;
      bool MxIBELghSY = false;
      bool mAXApBTkyS = false;
      bool iGugMiDkUJ = false;
      bool zMWnbtTJtm = false;
      bool lzBwWXigAi = false;
      bool yWCkzGCboV = false;
      string YGThdKQlFP;
      string fIMRLRPBSI;
      string ysmALHNQGq;
      string hEeeJqPFjI;
      string sIIsXXtEwH;
      string tBPZzfSSVW;
      string wAfOgbdgIU;
      string CJRJOpjtbn;
      string MJUywgnEfc;
      string XwjNMsbtLw;
      string sMwaEjiFLI;
      string wYphKlfgRA;
      string mZBewFZDxU;
      string OpiftzNssL;
      string xirWgzFBCZ;
      string NtbKJTLQez;
      string KfIqtFoKmK;
      string XmmFDDhexw;
      string zXAWHUNXWZ;
      string nxNtBRaBba;
      if(YGThdKQlFP == sMwaEjiFLI){JuNrBZGVHF = true;}
      else if(sMwaEjiFLI == YGThdKQlFP){dekxlQaQqj = true;}
      if(fIMRLRPBSI == wYphKlfgRA){lfThAlekiY = true;}
      else if(wYphKlfgRA == fIMRLRPBSI){pkbFbHNaWo = true;}
      if(ysmALHNQGq == mZBewFZDxU){qRnNWaYsEl = true;}
      else if(mZBewFZDxU == ysmALHNQGq){rcLzzThTUK = true;}
      if(hEeeJqPFjI == OpiftzNssL){cfCVmEJIVg = true;}
      else if(OpiftzNssL == hEeeJqPFjI){XGgpkSHBDh = true;}
      if(sIIsXXtEwH == xirWgzFBCZ){NrQQnHcftO = true;}
      else if(xirWgzFBCZ == sIIsXXtEwH){MxIBELghSY = true;}
      if(tBPZzfSSVW == NtbKJTLQez){mpRCjrHAUa = true;}
      else if(NtbKJTLQez == tBPZzfSSVW){mAXApBTkyS = true;}
      if(wAfOgbdgIU == KfIqtFoKmK){BMQVbFcxtw = true;}
      else if(KfIqtFoKmK == wAfOgbdgIU){iGugMiDkUJ = true;}
      if(CJRJOpjtbn == XmmFDDhexw){cVOBLzBWYO = true;}
      if(MJUywgnEfc == zXAWHUNXWZ){yszpzmNAlu = true;}
      if(XwjNMsbtLw == nxNtBRaBba){wInufAatoh = true;}
      while(XmmFDDhexw == CJRJOpjtbn){zMWnbtTJtm = true;}
      while(zXAWHUNXWZ == zXAWHUNXWZ){lzBwWXigAi = true;}
      while(nxNtBRaBba == nxNtBRaBba){yWCkzGCboV = true;}
      if(JuNrBZGVHF == true){JuNrBZGVHF = false;}
      if(lfThAlekiY == true){lfThAlekiY = false;}
      if(qRnNWaYsEl == true){qRnNWaYsEl = false;}
      if(cfCVmEJIVg == true){cfCVmEJIVg = false;}
      if(NrQQnHcftO == true){NrQQnHcftO = false;}
      if(mpRCjrHAUa == true){mpRCjrHAUa = false;}
      if(BMQVbFcxtw == true){BMQVbFcxtw = false;}
      if(cVOBLzBWYO == true){cVOBLzBWYO = false;}
      if(yszpzmNAlu == true){yszpzmNAlu = false;}
      if(wInufAatoh == true){wInufAatoh = false;}
      if(dekxlQaQqj == true){dekxlQaQqj = false;}
      if(pkbFbHNaWo == true){pkbFbHNaWo = false;}
      if(rcLzzThTUK == true){rcLzzThTUK = false;}
      if(XGgpkSHBDh == true){XGgpkSHBDh = false;}
      if(MxIBELghSY == true){MxIBELghSY = false;}
      if(mAXApBTkyS == true){mAXApBTkyS = false;}
      if(iGugMiDkUJ == true){iGugMiDkUJ = false;}
      if(zMWnbtTJtm == true){zMWnbtTJtm = false;}
      if(lzBwWXigAi == true){lzBwWXigAi = false;}
      if(yWCkzGCboV == true){yWCkzGCboV = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QUBNGFWBOD
{ 
  void rjXsYXnuiu()
  { 
      bool WjViARGIRe = false;
      bool SpTEaQpgXG = false;
      bool KTMWmAYWKZ = false;
      bool lfFyAIHHXH = false;
      bool JXgXSQeLUT = false;
      bool QVchqDWywz = false;
      bool riyqrlkVDR = false;
      bool IiIQxmloDV = false;
      bool UNCqJQDXHw = false;
      bool kJLYPZYoeg = false;
      bool fJHCKgNMjE = false;
      bool RguZddgGhb = false;
      bool iNOHgzWiAW = false;
      bool NwKWpLGzgc = false;
      bool DqmDeENltk = false;
      bool sSHCxRuAVK = false;
      bool JdLMpUWXiS = false;
      bool XhZFYrZKbb = false;
      bool lkoPdunHYX = false;
      bool PGcVSYeZMJ = false;
      string CnIRaOyTgu;
      string xYKeMiXZUO;
      string LIDlDELmLV;
      string gNTfPrHomY;
      string xOpiAkUcHl;
      string moZsItkWOV;
      string DKHKNjFdOT;
      string biluRwRTlS;
      string iheYirmZXV;
      string HjaQVlHfUP;
      string fqNnKDCebL;
      string cRkumUfhaV;
      string weGsMqIUZe;
      string okyVlrUqOf;
      string oHxLsozedA;
      string GYEzTAkUaS;
      string uhFwWqSPIt;
      string AoSugngtAW;
      string rdqYYPgzCl;
      string HzPukGCAoB;
      if(CnIRaOyTgu == fqNnKDCebL){WjViARGIRe = true;}
      else if(fqNnKDCebL == CnIRaOyTgu){fJHCKgNMjE = true;}
      if(xYKeMiXZUO == cRkumUfhaV){SpTEaQpgXG = true;}
      else if(cRkumUfhaV == xYKeMiXZUO){RguZddgGhb = true;}
      if(LIDlDELmLV == weGsMqIUZe){KTMWmAYWKZ = true;}
      else if(weGsMqIUZe == LIDlDELmLV){iNOHgzWiAW = true;}
      if(gNTfPrHomY == okyVlrUqOf){lfFyAIHHXH = true;}
      else if(okyVlrUqOf == gNTfPrHomY){NwKWpLGzgc = true;}
      if(xOpiAkUcHl == oHxLsozedA){JXgXSQeLUT = true;}
      else if(oHxLsozedA == xOpiAkUcHl){DqmDeENltk = true;}
      if(moZsItkWOV == GYEzTAkUaS){QVchqDWywz = true;}
      else if(GYEzTAkUaS == moZsItkWOV){sSHCxRuAVK = true;}
      if(DKHKNjFdOT == uhFwWqSPIt){riyqrlkVDR = true;}
      else if(uhFwWqSPIt == DKHKNjFdOT){JdLMpUWXiS = true;}
      if(biluRwRTlS == AoSugngtAW){IiIQxmloDV = true;}
      if(iheYirmZXV == rdqYYPgzCl){UNCqJQDXHw = true;}
      if(HjaQVlHfUP == HzPukGCAoB){kJLYPZYoeg = true;}
      while(AoSugngtAW == biluRwRTlS){XhZFYrZKbb = true;}
      while(rdqYYPgzCl == rdqYYPgzCl){lkoPdunHYX = true;}
      while(HzPukGCAoB == HzPukGCAoB){PGcVSYeZMJ = true;}
      if(WjViARGIRe == true){WjViARGIRe = false;}
      if(SpTEaQpgXG == true){SpTEaQpgXG = false;}
      if(KTMWmAYWKZ == true){KTMWmAYWKZ = false;}
      if(lfFyAIHHXH == true){lfFyAIHHXH = false;}
      if(JXgXSQeLUT == true){JXgXSQeLUT = false;}
      if(QVchqDWywz == true){QVchqDWywz = false;}
      if(riyqrlkVDR == true){riyqrlkVDR = false;}
      if(IiIQxmloDV == true){IiIQxmloDV = false;}
      if(UNCqJQDXHw == true){UNCqJQDXHw = false;}
      if(kJLYPZYoeg == true){kJLYPZYoeg = false;}
      if(fJHCKgNMjE == true){fJHCKgNMjE = false;}
      if(RguZddgGhb == true){RguZddgGhb = false;}
      if(iNOHgzWiAW == true){iNOHgzWiAW = false;}
      if(NwKWpLGzgc == true){NwKWpLGzgc = false;}
      if(DqmDeENltk == true){DqmDeENltk = false;}
      if(sSHCxRuAVK == true){sSHCxRuAVK = false;}
      if(JdLMpUWXiS == true){JdLMpUWXiS = false;}
      if(XhZFYrZKbb == true){XhZFYrZKbb = false;}
      if(lkoPdunHYX == true){lkoPdunHYX = false;}
      if(PGcVSYeZMJ == true){PGcVSYeZMJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class EDKIAQFXCH
{ 
  void grrklQmgEz()
  { 
      bool QISGkpFNdo = false;
      bool FcmIfENZWw = false;
      bool PVAYzsFraZ = false;
      bool WylbZNXfWy = false;
      bool pZkZcheJDp = false;
      bool iiDLHtItGr = false;
      bool kNmERGpKhR = false;
      bool fhCXFojauR = false;
      bool NunkpmtaUf = false;
      bool FdYeNcmgHW = false;
      bool qTblJBfeFR = false;
      bool JgyodoYBNe = false;
      bool LPoCMBJWKb = false;
      bool BDSYZsTCao = false;
      bool GOUDVXfiBX = false;
      bool DlScKbdYZH = false;
      bool qJUcPGOYOj = false;
      bool VWaYXwtlgn = false;
      bool VaVmYPjpOy = false;
      bool OtbSxxtrmO = false;
      string PXsfXrsxmq;
      string oipVrLbMPE;
      string WXgUHhIwyZ;
      string TmVXsuuRzw;
      string zuHttEjpnO;
      string XRHTzqpYXp;
      string iwyNXCNScx;
      string QKMSFaQmZD;
      string ySIjCCeBXC;
      string EVsJNCwsWq;
      string pQNqxMEnOX;
      string aCyTqQnOKU;
      string WLtartQDFC;
      string eWQiMgFQlp;
      string xqlAaCXZRw;
      string NhiBTeWlyC;
      string uAhLsDkOGK;
      string LXbUWeZszp;
      string WqlIownPOh;
      string KSRjtxIaHW;
      if(PXsfXrsxmq == pQNqxMEnOX){QISGkpFNdo = true;}
      else if(pQNqxMEnOX == PXsfXrsxmq){qTblJBfeFR = true;}
      if(oipVrLbMPE == aCyTqQnOKU){FcmIfENZWw = true;}
      else if(aCyTqQnOKU == oipVrLbMPE){JgyodoYBNe = true;}
      if(WXgUHhIwyZ == WLtartQDFC){PVAYzsFraZ = true;}
      else if(WLtartQDFC == WXgUHhIwyZ){LPoCMBJWKb = true;}
      if(TmVXsuuRzw == eWQiMgFQlp){WylbZNXfWy = true;}
      else if(eWQiMgFQlp == TmVXsuuRzw){BDSYZsTCao = true;}
      if(zuHttEjpnO == xqlAaCXZRw){pZkZcheJDp = true;}
      else if(xqlAaCXZRw == zuHttEjpnO){GOUDVXfiBX = true;}
      if(XRHTzqpYXp == NhiBTeWlyC){iiDLHtItGr = true;}
      else if(NhiBTeWlyC == XRHTzqpYXp){DlScKbdYZH = true;}
      if(iwyNXCNScx == uAhLsDkOGK){kNmERGpKhR = true;}
      else if(uAhLsDkOGK == iwyNXCNScx){qJUcPGOYOj = true;}
      if(QKMSFaQmZD == LXbUWeZszp){fhCXFojauR = true;}
      if(ySIjCCeBXC == WqlIownPOh){NunkpmtaUf = true;}
      if(EVsJNCwsWq == KSRjtxIaHW){FdYeNcmgHW = true;}
      while(LXbUWeZszp == QKMSFaQmZD){VWaYXwtlgn = true;}
      while(WqlIownPOh == WqlIownPOh){VaVmYPjpOy = true;}
      while(KSRjtxIaHW == KSRjtxIaHW){OtbSxxtrmO = true;}
      if(QISGkpFNdo == true){QISGkpFNdo = false;}
      if(FcmIfENZWw == true){FcmIfENZWw = false;}
      if(PVAYzsFraZ == true){PVAYzsFraZ = false;}
      if(WylbZNXfWy == true){WylbZNXfWy = false;}
      if(pZkZcheJDp == true){pZkZcheJDp = false;}
      if(iiDLHtItGr == true){iiDLHtItGr = false;}
      if(kNmERGpKhR == true){kNmERGpKhR = false;}
      if(fhCXFojauR == true){fhCXFojauR = false;}
      if(NunkpmtaUf == true){NunkpmtaUf = false;}
      if(FdYeNcmgHW == true){FdYeNcmgHW = false;}
      if(qTblJBfeFR == true){qTblJBfeFR = false;}
      if(JgyodoYBNe == true){JgyodoYBNe = false;}
      if(LPoCMBJWKb == true){LPoCMBJWKb = false;}
      if(BDSYZsTCao == true){BDSYZsTCao = false;}
      if(GOUDVXfiBX == true){GOUDVXfiBX = false;}
      if(DlScKbdYZH == true){DlScKbdYZH = false;}
      if(qJUcPGOYOj == true){qJUcPGOYOj = false;}
      if(VWaYXwtlgn == true){VWaYXwtlgn = false;}
      if(VaVmYPjpOy == true){VaVmYPjpOy = false;}
      if(OtbSxxtrmO == true){OtbSxxtrmO = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PUTPTRNPQB
{ 
  void VhZaiiRQbR()
  { 
      bool QisHjDiKsx = false;
      bool sBRzwLWYmf = false;
      bool sOfUumuddb = false;
      bool JboxiPfsMr = false;
      bool LMtozQsHuD = false;
      bool FISmSCCXMw = false;
      bool IEfRnRLUPg = false;
      bool ecLtUfUqUd = false;
      bool tmiaVBJCdg = false;
      bool MHgPjGgbBh = false;
      bool FYrOXcwXSm = false;
      bool gLDiCVVEul = false;
      bool HJpiTWwwKN = false;
      bool yrplXVuTDs = false;
      bool EkiqzAEyMN = false;
      bool jbgOZwhJtB = false;
      bool gANFnoVQKg = false;
      bool ZCBBhWZziT = false;
      bool RxiwcWLaLb = false;
      bool mlZCMEXdCK = false;
      string pdIzVyKjIY;
      string tTwsUMDboO;
      string eZwxElSZlA;
      string WuZSkpKitu;
      string yNDFiosUyA;
      string kbsAtPgKui;
      string fjxHBytdHd;
      string CTgsPRWqOO;
      string sxalPuCYXV;
      string JznLPOQuEk;
      string uPMUyQtzjc;
      string rowgphIKrF;
      string uUFlUCfVdB;
      string AalcSckGnz;
      string ADnucTDGUg;
      string FFDypZKazu;
      string jXVibQCTNN;
      string xBWsErgJIA;
      string zYBwwuXnms;
      string RKGrOchFTL;
      if(pdIzVyKjIY == uPMUyQtzjc){QisHjDiKsx = true;}
      else if(uPMUyQtzjc == pdIzVyKjIY){FYrOXcwXSm = true;}
      if(tTwsUMDboO == rowgphIKrF){sBRzwLWYmf = true;}
      else if(rowgphIKrF == tTwsUMDboO){gLDiCVVEul = true;}
      if(eZwxElSZlA == uUFlUCfVdB){sOfUumuddb = true;}
      else if(uUFlUCfVdB == eZwxElSZlA){HJpiTWwwKN = true;}
      if(WuZSkpKitu == AalcSckGnz){JboxiPfsMr = true;}
      else if(AalcSckGnz == WuZSkpKitu){yrplXVuTDs = true;}
      if(yNDFiosUyA == ADnucTDGUg){LMtozQsHuD = true;}
      else if(ADnucTDGUg == yNDFiosUyA){EkiqzAEyMN = true;}
      if(kbsAtPgKui == FFDypZKazu){FISmSCCXMw = true;}
      else if(FFDypZKazu == kbsAtPgKui){jbgOZwhJtB = true;}
      if(fjxHBytdHd == jXVibQCTNN){IEfRnRLUPg = true;}
      else if(jXVibQCTNN == fjxHBytdHd){gANFnoVQKg = true;}
      if(CTgsPRWqOO == xBWsErgJIA){ecLtUfUqUd = true;}
      if(sxalPuCYXV == zYBwwuXnms){tmiaVBJCdg = true;}
      if(JznLPOQuEk == RKGrOchFTL){MHgPjGgbBh = true;}
      while(xBWsErgJIA == CTgsPRWqOO){ZCBBhWZziT = true;}
      while(zYBwwuXnms == zYBwwuXnms){RxiwcWLaLb = true;}
      while(RKGrOchFTL == RKGrOchFTL){mlZCMEXdCK = true;}
      if(QisHjDiKsx == true){QisHjDiKsx = false;}
      if(sBRzwLWYmf == true){sBRzwLWYmf = false;}
      if(sOfUumuddb == true){sOfUumuddb = false;}
      if(JboxiPfsMr == true){JboxiPfsMr = false;}
      if(LMtozQsHuD == true){LMtozQsHuD = false;}
      if(FISmSCCXMw == true){FISmSCCXMw = false;}
      if(IEfRnRLUPg == true){IEfRnRLUPg = false;}
      if(ecLtUfUqUd == true){ecLtUfUqUd = false;}
      if(tmiaVBJCdg == true){tmiaVBJCdg = false;}
      if(MHgPjGgbBh == true){MHgPjGgbBh = false;}
      if(FYrOXcwXSm == true){FYrOXcwXSm = false;}
      if(gLDiCVVEul == true){gLDiCVVEul = false;}
      if(HJpiTWwwKN == true){HJpiTWwwKN = false;}
      if(yrplXVuTDs == true){yrplXVuTDs = false;}
      if(EkiqzAEyMN == true){EkiqzAEyMN = false;}
      if(jbgOZwhJtB == true){jbgOZwhJtB = false;}
      if(gANFnoVQKg == true){gANFnoVQKg = false;}
      if(ZCBBhWZziT == true){ZCBBhWZziT = false;}
      if(RxiwcWLaLb == true){RxiwcWLaLb = false;}
      if(mlZCMEXdCK == true){mlZCMEXdCK = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PFZTRUGQDO
{ 
  void FsBnoymkbu()
  { 
      bool DNdmJBoKcX = false;
      bool SlRPkNWpRS = false;
      bool DctOJXuNAr = false;
      bool OWuSxynJji = false;
      bool jznnszNWYh = false;
      bool JngjhZTOcl = false;
      bool FaKkJwpyRm = false;
      bool YNFhhEnFVK = false;
      bool JYoRhapKjX = false;
      bool eYgycJoDex = false;
      bool IPEkIwhyCd = false;
      bool kNsRbglrDi = false;
      bool GfNtHzfHEY = false;
      bool PkaYBtTDJk = false;
      bool sqIGlRfOVw = false;
      bool XwzwpcBKdI = false;
      bool QZscOAmTQh = false;
      bool MhiSoOWHIp = false;
      bool TRlIBSkehY = false;
      bool sJZAUSyerl = false;
      string CRgIGWwPmK;
      string rlCpkaYqZO;
      string pbEtUrosyk;
      string gaojFYqUMz;
      string XKfHpIuLaM;
      string GeSOKaMmmN;
      string hDRBcQxSjL;
      string iEeZpfuxDL;
      string xcRkdCIZZe;
      string MDgyRUsedZ;
      string LcbOcCAsXe;
      string eOjbQEkfXa;
      string IWsFyfWfHU;
      string lQTdAUBCHa;
      string BPBuAyTFhM;
      string jMGCQniWQx;
      string eUkDrNtGau;
      string ChXwYsUxFA;
      string LRxtkYJRMM;
      string kelKugdyCl;
      if(CRgIGWwPmK == LcbOcCAsXe){DNdmJBoKcX = true;}
      else if(LcbOcCAsXe == CRgIGWwPmK){IPEkIwhyCd = true;}
      if(rlCpkaYqZO == eOjbQEkfXa){SlRPkNWpRS = true;}
      else if(eOjbQEkfXa == rlCpkaYqZO){kNsRbglrDi = true;}
      if(pbEtUrosyk == IWsFyfWfHU){DctOJXuNAr = true;}
      else if(IWsFyfWfHU == pbEtUrosyk){GfNtHzfHEY = true;}
      if(gaojFYqUMz == lQTdAUBCHa){OWuSxynJji = true;}
      else if(lQTdAUBCHa == gaojFYqUMz){PkaYBtTDJk = true;}
      if(XKfHpIuLaM == BPBuAyTFhM){jznnszNWYh = true;}
      else if(BPBuAyTFhM == XKfHpIuLaM){sqIGlRfOVw = true;}
      if(GeSOKaMmmN == jMGCQniWQx){JngjhZTOcl = true;}
      else if(jMGCQniWQx == GeSOKaMmmN){XwzwpcBKdI = true;}
      if(hDRBcQxSjL == eUkDrNtGau){FaKkJwpyRm = true;}
      else if(eUkDrNtGau == hDRBcQxSjL){QZscOAmTQh = true;}
      if(iEeZpfuxDL == ChXwYsUxFA){YNFhhEnFVK = true;}
      if(xcRkdCIZZe == LRxtkYJRMM){JYoRhapKjX = true;}
      if(MDgyRUsedZ == kelKugdyCl){eYgycJoDex = true;}
      while(ChXwYsUxFA == iEeZpfuxDL){MhiSoOWHIp = true;}
      while(LRxtkYJRMM == LRxtkYJRMM){TRlIBSkehY = true;}
      while(kelKugdyCl == kelKugdyCl){sJZAUSyerl = true;}
      if(DNdmJBoKcX == true){DNdmJBoKcX = false;}
      if(SlRPkNWpRS == true){SlRPkNWpRS = false;}
      if(DctOJXuNAr == true){DctOJXuNAr = false;}
      if(OWuSxynJji == true){OWuSxynJji = false;}
      if(jznnszNWYh == true){jznnszNWYh = false;}
      if(JngjhZTOcl == true){JngjhZTOcl = false;}
      if(FaKkJwpyRm == true){FaKkJwpyRm = false;}
      if(YNFhhEnFVK == true){YNFhhEnFVK = false;}
      if(JYoRhapKjX == true){JYoRhapKjX = false;}
      if(eYgycJoDex == true){eYgycJoDex = false;}
      if(IPEkIwhyCd == true){IPEkIwhyCd = false;}
      if(kNsRbglrDi == true){kNsRbglrDi = false;}
      if(GfNtHzfHEY == true){GfNtHzfHEY = false;}
      if(PkaYBtTDJk == true){PkaYBtTDJk = false;}
      if(sqIGlRfOVw == true){sqIGlRfOVw = false;}
      if(XwzwpcBKdI == true){XwzwpcBKdI = false;}
      if(QZscOAmTQh == true){QZscOAmTQh = false;}
      if(MhiSoOWHIp == true){MhiSoOWHIp = false;}
      if(TRlIBSkehY == true){TRlIBSkehY = false;}
      if(sJZAUSyerl == true){sJZAUSyerl = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LXOTDRVDTM
{ 
  void edcpTBpuce()
  { 
      bool RUbxDQxLek = false;
      bool znRJoilGjT = false;
      bool TWXKlaZSdu = false;
      bool dHYVtobBiU = false;
      bool NJlNnQwgyD = false;
      bool ZpGFntwQLc = false;
      bool qehiHqOuPz = false;
      bool HkXPFKkJVU = false;
      bool drTaVmAThA = false;
      bool rKpGciaCGw = false;
      bool CBQywoRpzd = false;
      bool ImtpyyKGNT = false;
      bool quNPGwtRmW = false;
      bool iiDYScbVdf = false;
      bool VKTEqFQAHd = false;
      bool XUnlFGmBqz = false;
      bool uADbtZiUXV = false;
      bool WobhyfbJXB = false;
      bool ftJIzOuRSL = false;
      bool oZdekUGqJR = false;
      string YZSLrGiBbB;
      string lqtHLdmKyU;
      string iReuaXXEUw;
      string SHZustzxtb;
      string oazdHconEg;
      string GqtVzGtWjT;
      string idffhdRyOQ;
      string dpPVeHSAny;
      string ijAnoaNEul;
      string zbMarLtTuc;
      string LpYmCPbXpr;
      string nsilkzdqpH;
      string QMCcRCdodT;
      string NyFLmsKMlY;
      string zpNDxFraor;
      string hCcnLymwVz;
      string llsWdonXzr;
      string oQFQIazzeL;
      string KDzllKVQzn;
      string FOqmmMxqZI;
      if(YZSLrGiBbB == LpYmCPbXpr){RUbxDQxLek = true;}
      else if(LpYmCPbXpr == YZSLrGiBbB){CBQywoRpzd = true;}
      if(lqtHLdmKyU == nsilkzdqpH){znRJoilGjT = true;}
      else if(nsilkzdqpH == lqtHLdmKyU){ImtpyyKGNT = true;}
      if(iReuaXXEUw == QMCcRCdodT){TWXKlaZSdu = true;}
      else if(QMCcRCdodT == iReuaXXEUw){quNPGwtRmW = true;}
      if(SHZustzxtb == NyFLmsKMlY){dHYVtobBiU = true;}
      else if(NyFLmsKMlY == SHZustzxtb){iiDYScbVdf = true;}
      if(oazdHconEg == zpNDxFraor){NJlNnQwgyD = true;}
      else if(zpNDxFraor == oazdHconEg){VKTEqFQAHd = true;}
      if(GqtVzGtWjT == hCcnLymwVz){ZpGFntwQLc = true;}
      else if(hCcnLymwVz == GqtVzGtWjT){XUnlFGmBqz = true;}
      if(idffhdRyOQ == llsWdonXzr){qehiHqOuPz = true;}
      else if(llsWdonXzr == idffhdRyOQ){uADbtZiUXV = true;}
      if(dpPVeHSAny == oQFQIazzeL){HkXPFKkJVU = true;}
      if(ijAnoaNEul == KDzllKVQzn){drTaVmAThA = true;}
      if(zbMarLtTuc == FOqmmMxqZI){rKpGciaCGw = true;}
      while(oQFQIazzeL == dpPVeHSAny){WobhyfbJXB = true;}
      while(KDzllKVQzn == KDzllKVQzn){ftJIzOuRSL = true;}
      while(FOqmmMxqZI == FOqmmMxqZI){oZdekUGqJR = true;}
      if(RUbxDQxLek == true){RUbxDQxLek = false;}
      if(znRJoilGjT == true){znRJoilGjT = false;}
      if(TWXKlaZSdu == true){TWXKlaZSdu = false;}
      if(dHYVtobBiU == true){dHYVtobBiU = false;}
      if(NJlNnQwgyD == true){NJlNnQwgyD = false;}
      if(ZpGFntwQLc == true){ZpGFntwQLc = false;}
      if(qehiHqOuPz == true){qehiHqOuPz = false;}
      if(HkXPFKkJVU == true){HkXPFKkJVU = false;}
      if(drTaVmAThA == true){drTaVmAThA = false;}
      if(rKpGciaCGw == true){rKpGciaCGw = false;}
      if(CBQywoRpzd == true){CBQywoRpzd = false;}
      if(ImtpyyKGNT == true){ImtpyyKGNT = false;}
      if(quNPGwtRmW == true){quNPGwtRmW = false;}
      if(iiDYScbVdf == true){iiDYScbVdf = false;}
      if(VKTEqFQAHd == true){VKTEqFQAHd = false;}
      if(XUnlFGmBqz == true){XUnlFGmBqz = false;}
      if(uADbtZiUXV == true){uADbtZiUXV = false;}
      if(WobhyfbJXB == true){WobhyfbJXB = false;}
      if(ftJIzOuRSL == true){ftJIzOuRSL = false;}
      if(oZdekUGqJR == true){oZdekUGqJR = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class OCPIYGBGHN
{ 
  void SzowGuLJHT()
  { 
      bool TMmFgSEwle = false;
      bool HRpqnMnVXJ = false;
      bool frggeqxTaC = false;
      bool iPfRMKmWLF = false;
      bool fOBUZJfELn = false;
      bool QHhmUafGao = false;
      bool VkUiwrlXdc = false;
      bool ZRKRdQpLGU = false;
      bool KrHmlbgrxk = false;
      bool efuVjtsRYh = false;
      bool XHZiNyxQJD = false;
      bool kTzGdccgwT = false;
      bool KtarytVngi = false;
      bool XXlYwCSYYn = false;
      bool ePitAKforH = false;
      bool RLVEWdggjT = false;
      bool bIiZeMgaJa = false;
      bool KEHIouKgrm = false;
      bool NgwZRLCgBS = false;
      bool VknFLVgJOm = false;
      string kzDGlUHRpO;
      string PoYoXWMmxZ;
      string gkflVTiUzU;
      string KRfIQfkKIX;
      string NUKauKfnwM;
      string QjZuOzSQjR;
      string llRUDxIPYr;
      string kCeYMLZFpL;
      string ntXmDarktQ;
      string MTZplaDpWx;
      string BQPDbOFAhC;
      string ZNZYdqNeQc;
      string FXUIfQbnll;
      string IihsKWdTKn;
      string UrCqpzsBMt;
      string MXYtrcuHXK;
      string mQNwtpfgNG;
      string NsTcHMVcRf;
      string WZbrYjKnAg;
      string bVwABEdGxA;
      if(kzDGlUHRpO == BQPDbOFAhC){TMmFgSEwle = true;}
      else if(BQPDbOFAhC == kzDGlUHRpO){XHZiNyxQJD = true;}
      if(PoYoXWMmxZ == ZNZYdqNeQc){HRpqnMnVXJ = true;}
      else if(ZNZYdqNeQc == PoYoXWMmxZ){kTzGdccgwT = true;}
      if(gkflVTiUzU == FXUIfQbnll){frggeqxTaC = true;}
      else if(FXUIfQbnll == gkflVTiUzU){KtarytVngi = true;}
      if(KRfIQfkKIX == IihsKWdTKn){iPfRMKmWLF = true;}
      else if(IihsKWdTKn == KRfIQfkKIX){XXlYwCSYYn = true;}
      if(NUKauKfnwM == UrCqpzsBMt){fOBUZJfELn = true;}
      else if(UrCqpzsBMt == NUKauKfnwM){ePitAKforH = true;}
      if(QjZuOzSQjR == MXYtrcuHXK){QHhmUafGao = true;}
      else if(MXYtrcuHXK == QjZuOzSQjR){RLVEWdggjT = true;}
      if(llRUDxIPYr == mQNwtpfgNG){VkUiwrlXdc = true;}
      else if(mQNwtpfgNG == llRUDxIPYr){bIiZeMgaJa = true;}
      if(kCeYMLZFpL == NsTcHMVcRf){ZRKRdQpLGU = true;}
      if(ntXmDarktQ == WZbrYjKnAg){KrHmlbgrxk = true;}
      if(MTZplaDpWx == bVwABEdGxA){efuVjtsRYh = true;}
      while(NsTcHMVcRf == kCeYMLZFpL){KEHIouKgrm = true;}
      while(WZbrYjKnAg == WZbrYjKnAg){NgwZRLCgBS = true;}
      while(bVwABEdGxA == bVwABEdGxA){VknFLVgJOm = true;}
      if(TMmFgSEwle == true){TMmFgSEwle = false;}
      if(HRpqnMnVXJ == true){HRpqnMnVXJ = false;}
      if(frggeqxTaC == true){frggeqxTaC = false;}
      if(iPfRMKmWLF == true){iPfRMKmWLF = false;}
      if(fOBUZJfELn == true){fOBUZJfELn = false;}
      if(QHhmUafGao == true){QHhmUafGao = false;}
      if(VkUiwrlXdc == true){VkUiwrlXdc = false;}
      if(ZRKRdQpLGU == true){ZRKRdQpLGU = false;}
      if(KrHmlbgrxk == true){KrHmlbgrxk = false;}
      if(efuVjtsRYh == true){efuVjtsRYh = false;}
      if(XHZiNyxQJD == true){XHZiNyxQJD = false;}
      if(kTzGdccgwT == true){kTzGdccgwT = false;}
      if(KtarytVngi == true){KtarytVngi = false;}
      if(XXlYwCSYYn == true){XXlYwCSYYn = false;}
      if(ePitAKforH == true){ePitAKforH = false;}
      if(RLVEWdggjT == true){RLVEWdggjT = false;}
      if(bIiZeMgaJa == true){bIiZeMgaJa = false;}
      if(KEHIouKgrm == true){KEHIouKgrm = false;}
      if(NgwZRLCgBS == true){NgwZRLCgBS = false;}
      if(VknFLVgJOm == true){VknFLVgJOm = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PWNXBEVWAF
{ 
  void QOugrtFoaY()
  { 
      bool wBYwTrZVPi = false;
      bool jkfNytCGRg = false;
      bool eZbxEDBJgd = false;
      bool CalJpLBYma = false;
      bool awEStIgFwT = false;
      bool dmHQdWYbqQ = false;
      bool qPnlfIyVWl = false;
      bool rcSpFiUJCu = false;
      bool VuNMzPTVfT = false;
      bool mZgWiKHSll = false;
      bool KKiJbBTRKt = false;
      bool BPYwtwCalQ = false;
      bool ocpDzRuwHu = false;
      bool aCWQkLLDky = false;
      bool uCLkRWBkcm = false;
      bool HfgVxhUfJJ = false;
      bool FXJOTzrHKi = false;
      bool jzpmUmQFkc = false;
      bool QKSUntVBTD = false;
      bool pfsasXrwQS = false;
      string yLwxBjgPNW;
      string zMkORzUxia;
      string hlkUsoWqtC;
      string LeacAGQyVb;
      string rLGAEhqlqk;
      string UQOAjMZaDL;
      string NoULKUlHNL;
      string hWaHiWekQL;
      string yjAJguPasn;
      string fptJOCpupj;
      string uiXfDxpHQN;
      string goKrNcoLdP;
      string EKOZercRsf;
      string fiugPOLUGi;
      string EIJeGqeBuk;
      string MeQZekUyBL;
      string UXOjhxtdbz;
      string QUEuKwAiSm;
      string MaVHSASUBe;
      string tNTuNgsYxI;
      if(yLwxBjgPNW == uiXfDxpHQN){wBYwTrZVPi = true;}
      else if(uiXfDxpHQN == yLwxBjgPNW){KKiJbBTRKt = true;}
      if(zMkORzUxia == goKrNcoLdP){jkfNytCGRg = true;}
      else if(goKrNcoLdP == zMkORzUxia){BPYwtwCalQ = true;}
      if(hlkUsoWqtC == EKOZercRsf){eZbxEDBJgd = true;}
      else if(EKOZercRsf == hlkUsoWqtC){ocpDzRuwHu = true;}
      if(LeacAGQyVb == fiugPOLUGi){CalJpLBYma = true;}
      else if(fiugPOLUGi == LeacAGQyVb){aCWQkLLDky = true;}
      if(rLGAEhqlqk == EIJeGqeBuk){awEStIgFwT = true;}
      else if(EIJeGqeBuk == rLGAEhqlqk){uCLkRWBkcm = true;}
      if(UQOAjMZaDL == MeQZekUyBL){dmHQdWYbqQ = true;}
      else if(MeQZekUyBL == UQOAjMZaDL){HfgVxhUfJJ = true;}
      if(NoULKUlHNL == UXOjhxtdbz){qPnlfIyVWl = true;}
      else if(UXOjhxtdbz == NoULKUlHNL){FXJOTzrHKi = true;}
      if(hWaHiWekQL == QUEuKwAiSm){rcSpFiUJCu = true;}
      if(yjAJguPasn == MaVHSASUBe){VuNMzPTVfT = true;}
      if(fptJOCpupj == tNTuNgsYxI){mZgWiKHSll = true;}
      while(QUEuKwAiSm == hWaHiWekQL){jzpmUmQFkc = true;}
      while(MaVHSASUBe == MaVHSASUBe){QKSUntVBTD = true;}
      while(tNTuNgsYxI == tNTuNgsYxI){pfsasXrwQS = true;}
      if(wBYwTrZVPi == true){wBYwTrZVPi = false;}
      if(jkfNytCGRg == true){jkfNytCGRg = false;}
      if(eZbxEDBJgd == true){eZbxEDBJgd = false;}
      if(CalJpLBYma == true){CalJpLBYma = false;}
      if(awEStIgFwT == true){awEStIgFwT = false;}
      if(dmHQdWYbqQ == true){dmHQdWYbqQ = false;}
      if(qPnlfIyVWl == true){qPnlfIyVWl = false;}
      if(rcSpFiUJCu == true){rcSpFiUJCu = false;}
      if(VuNMzPTVfT == true){VuNMzPTVfT = false;}
      if(mZgWiKHSll == true){mZgWiKHSll = false;}
      if(KKiJbBTRKt == true){KKiJbBTRKt = false;}
      if(BPYwtwCalQ == true){BPYwtwCalQ = false;}
      if(ocpDzRuwHu == true){ocpDzRuwHu = false;}
      if(aCWQkLLDky == true){aCWQkLLDky = false;}
      if(uCLkRWBkcm == true){uCLkRWBkcm = false;}
      if(HfgVxhUfJJ == true){HfgVxhUfJJ = false;}
      if(FXJOTzrHKi == true){FXJOTzrHKi = false;}
      if(jzpmUmQFkc == true){jzpmUmQFkc = false;}
      if(QKSUntVBTD == true){QKSUntVBTD = false;}
      if(pfsasXrwQS == true){pfsasXrwQS = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QZEVZACRNF
{ 
  void GzLIDQHkII()
  { 
      bool kFPFBGDIGP = false;
      bool IubLIHZZsn = false;
      bool dpbEconrwj = false;
      bool TrCyzHgTVx = false;
      bool NASqTaKFPs = false;
      bool hXVFlucLcM = false;
      bool SKLVuQsdLA = false;
      bool ZFDuZdWjjB = false;
      bool dicCCwVwLE = false;
      bool eVqJdDnwwD = false;
      bool hgsOScTZfI = false;
      bool XrbahjhJTz = false;
      bool twRHVENsRW = false;
      bool oanfTxoNiZ = false;
      bool bwQCflsPbt = false;
      bool UecxJlOlsO = false;
      bool OZIJSxEuHk = false;
      bool uTZtWyfPaj = false;
      bool KyRXBKyqWh = false;
      bool hCUUKBmGih = false;
      string UKrHpwVsqx;
      string dTmuVmzgBk;
      string fuCjCaJGMe;
      string NXcAbuVdiT;
      string rMWgAZYZJq;
      string WbeEJYHHEP;
      string mOgMjGplTZ;
      string QUygrSYUic;
      string BixUHksXpC;
      string mNZSgcfKxt;
      string imGiZBdxLq;
      string AmXbNDRLEy;
      string MSuQlDueAP;
      string lOXnsObNuG;
      string GuLzCAoyFc;
      string LUFygPqpxW;
      string FzxwFlOihz;
      string aEQxrRHwNk;
      string NxZXDRIYHc;
      string WlQXBHxOsP;
      if(UKrHpwVsqx == imGiZBdxLq){kFPFBGDIGP = true;}
      else if(imGiZBdxLq == UKrHpwVsqx){hgsOScTZfI = true;}
      if(dTmuVmzgBk == AmXbNDRLEy){IubLIHZZsn = true;}
      else if(AmXbNDRLEy == dTmuVmzgBk){XrbahjhJTz = true;}
      if(fuCjCaJGMe == MSuQlDueAP){dpbEconrwj = true;}
      else if(MSuQlDueAP == fuCjCaJGMe){twRHVENsRW = true;}
      if(NXcAbuVdiT == lOXnsObNuG){TrCyzHgTVx = true;}
      else if(lOXnsObNuG == NXcAbuVdiT){oanfTxoNiZ = true;}
      if(rMWgAZYZJq == GuLzCAoyFc){NASqTaKFPs = true;}
      else if(GuLzCAoyFc == rMWgAZYZJq){bwQCflsPbt = true;}
      if(WbeEJYHHEP == LUFygPqpxW){hXVFlucLcM = true;}
      else if(LUFygPqpxW == WbeEJYHHEP){UecxJlOlsO = true;}
      if(mOgMjGplTZ == FzxwFlOihz){SKLVuQsdLA = true;}
      else if(FzxwFlOihz == mOgMjGplTZ){OZIJSxEuHk = true;}
      if(QUygrSYUic == aEQxrRHwNk){ZFDuZdWjjB = true;}
      if(BixUHksXpC == NxZXDRIYHc){dicCCwVwLE = true;}
      if(mNZSgcfKxt == WlQXBHxOsP){eVqJdDnwwD = true;}
      while(aEQxrRHwNk == QUygrSYUic){uTZtWyfPaj = true;}
      while(NxZXDRIYHc == NxZXDRIYHc){KyRXBKyqWh = true;}
      while(WlQXBHxOsP == WlQXBHxOsP){hCUUKBmGih = true;}
      if(kFPFBGDIGP == true){kFPFBGDIGP = false;}
      if(IubLIHZZsn == true){IubLIHZZsn = false;}
      if(dpbEconrwj == true){dpbEconrwj = false;}
      if(TrCyzHgTVx == true){TrCyzHgTVx = false;}
      if(NASqTaKFPs == true){NASqTaKFPs = false;}
      if(hXVFlucLcM == true){hXVFlucLcM = false;}
      if(SKLVuQsdLA == true){SKLVuQsdLA = false;}
      if(ZFDuZdWjjB == true){ZFDuZdWjjB = false;}
      if(dicCCwVwLE == true){dicCCwVwLE = false;}
      if(eVqJdDnwwD == true){eVqJdDnwwD = false;}
      if(hgsOScTZfI == true){hgsOScTZfI = false;}
      if(XrbahjhJTz == true){XrbahjhJTz = false;}
      if(twRHVENsRW == true){twRHVENsRW = false;}
      if(oanfTxoNiZ == true){oanfTxoNiZ = false;}
      if(bwQCflsPbt == true){bwQCflsPbt = false;}
      if(UecxJlOlsO == true){UecxJlOlsO = false;}
      if(OZIJSxEuHk == true){OZIJSxEuHk = false;}
      if(uTZtWyfPaj == true){uTZtWyfPaj = false;}
      if(KyRXBKyqWh == true){KyRXBKyqWh = false;}
      if(hCUUKBmGih == true){hCUUKBmGih = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PSTNOLRKSP
{ 
  void bzZZnuafHV()
  { 
      bool LienyoCiaN = false;
      bool WPcDJQxRVC = false;
      bool PsaXJnnAIM = false;
      bool lsHDZQdRbx = false;
      bool EKAEdsOQRD = false;
      bool oiqPBSIHeK = false;
      bool ACZNbdIZds = false;
      bool mfpYyErWsP = false;
      bool YXhFXCBqBH = false;
      bool IVmQwCjmbA = false;
      bool kolYPJsRRw = false;
      bool akOOlcTUpZ = false;
      bool bQXVjRsYcw = false;
      bool UFtseikJoO = false;
      bool IiZFgWHXeT = false;
      bool hOwBXTRPny = false;
      bool QUoVbUqVjD = false;
      bool qKebQrqSoN = false;
      bool KUsJNFIrUL = false;
      bool QUVqxntEdL = false;
      string aZVbiEiGsI;
      string NzhWXURbDy;
      string FZgGlUEzXn;
      string LpGDoyDxnA;
      string zalNEzZXgB;
      string HEAqbBsOpN;
      string AjiEwWregY;
      string QTnDzdmTCl;
      string tfRVgGpOam;
      string FoHazPrhQh;
      string HtpIkmQwrx;
      string NYMDSURMWo;
      string gcEtSbMNEj;
      string lEDTeAoJkt;
      string psWdZgbDHd;
      string xeLaBdpwoJ;
      string ZDrIKIUsYS;
      string fIbczmGZTl;
      string zIFZlHsbRB;
      string JrTbTlBNSQ;
      if(aZVbiEiGsI == HtpIkmQwrx){LienyoCiaN = true;}
      else if(HtpIkmQwrx == aZVbiEiGsI){kolYPJsRRw = true;}
      if(NzhWXURbDy == NYMDSURMWo){WPcDJQxRVC = true;}
      else if(NYMDSURMWo == NzhWXURbDy){akOOlcTUpZ = true;}
      if(FZgGlUEzXn == gcEtSbMNEj){PsaXJnnAIM = true;}
      else if(gcEtSbMNEj == FZgGlUEzXn){bQXVjRsYcw = true;}
      if(LpGDoyDxnA == lEDTeAoJkt){lsHDZQdRbx = true;}
      else if(lEDTeAoJkt == LpGDoyDxnA){UFtseikJoO = true;}
      if(zalNEzZXgB == psWdZgbDHd){EKAEdsOQRD = true;}
      else if(psWdZgbDHd == zalNEzZXgB){IiZFgWHXeT = true;}
      if(HEAqbBsOpN == xeLaBdpwoJ){oiqPBSIHeK = true;}
      else if(xeLaBdpwoJ == HEAqbBsOpN){hOwBXTRPny = true;}
      if(AjiEwWregY == ZDrIKIUsYS){ACZNbdIZds = true;}
      else if(ZDrIKIUsYS == AjiEwWregY){QUoVbUqVjD = true;}
      if(QTnDzdmTCl == fIbczmGZTl){mfpYyErWsP = true;}
      if(tfRVgGpOam == zIFZlHsbRB){YXhFXCBqBH = true;}
      if(FoHazPrhQh == JrTbTlBNSQ){IVmQwCjmbA = true;}
      while(fIbczmGZTl == QTnDzdmTCl){qKebQrqSoN = true;}
      while(zIFZlHsbRB == zIFZlHsbRB){KUsJNFIrUL = true;}
      while(JrTbTlBNSQ == JrTbTlBNSQ){QUVqxntEdL = true;}
      if(LienyoCiaN == true){LienyoCiaN = false;}
      if(WPcDJQxRVC == true){WPcDJQxRVC = false;}
      if(PsaXJnnAIM == true){PsaXJnnAIM = false;}
      if(lsHDZQdRbx == true){lsHDZQdRbx = false;}
      if(EKAEdsOQRD == true){EKAEdsOQRD = false;}
      if(oiqPBSIHeK == true){oiqPBSIHeK = false;}
      if(ACZNbdIZds == true){ACZNbdIZds = false;}
      if(mfpYyErWsP == true){mfpYyErWsP = false;}
      if(YXhFXCBqBH == true){YXhFXCBqBH = false;}
      if(IVmQwCjmbA == true){IVmQwCjmbA = false;}
      if(kolYPJsRRw == true){kolYPJsRRw = false;}
      if(akOOlcTUpZ == true){akOOlcTUpZ = false;}
      if(bQXVjRsYcw == true){bQXVjRsYcw = false;}
      if(UFtseikJoO == true){UFtseikJoO = false;}
      if(IiZFgWHXeT == true){IiZFgWHXeT = false;}
      if(hOwBXTRPny == true){hOwBXTRPny = false;}
      if(QUoVbUqVjD == true){QUoVbUqVjD = false;}
      if(qKebQrqSoN == true){qKebQrqSoN = false;}
      if(KUsJNFIrUL == true){KUsJNFIrUL = false;}
      if(QUVqxntEdL == true){QUVqxntEdL = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class HXUVAFPFMC
{ 
  void lunwWkRmjF()
  { 
      bool KuqILYnPSz = false;
      bool sowAhJTlYT = false;
      bool SioVjDUXZD = false;
      bool jYRkBqIgWU = false;
      bool uHELiLuElu = false;
      bool ZQkwwaFVYH = false;
      bool DQoOOUWKzp = false;
      bool BlODYLedmX = false;
      bool IPkddTmVna = false;
      bool lmHOQeRpxO = false;
      bool ojFutmBhXk = false;
      bool wbhAyneLku = false;
      bool gAVXENeAls = false;
      bool snfXiYKpjp = false;
      bool RFyDeKEHPc = false;
      bool zGFBgnNgpf = false;
      bool IBudrHuKRZ = false;
      bool cnaucbCSst = false;
      bool IWjIslidqt = false;
      bool KdzDgQJTKW = false;
      string VWsXMEqzVb;
      string ZbOyBYhxGb;
      string NHVuaGXoaJ;
      string dcfcbLtsJX;
      string slBPwiJbfh;
      string qgsssNAywF;
      string fCYMywwkVo;
      string enmhibGFyq;
      string wuJrRwjljN;
      string LwwDrqMnRz;
      string lXFqDCsLKR;
      string HtzEJkrgtB;
      string MKBHcirjpr;
      string ORnyFXoSoP;
      string DYbSGQwqtH;
      string yftQmbWyNU;
      string KJURCpGBzk;
      string uZreynhhBM;
      string lApisiWWLc;
      string AqFTFtDJPP;
      if(VWsXMEqzVb == lXFqDCsLKR){KuqILYnPSz = true;}
      else if(lXFqDCsLKR == VWsXMEqzVb){ojFutmBhXk = true;}
      if(ZbOyBYhxGb == HtzEJkrgtB){sowAhJTlYT = true;}
      else if(HtzEJkrgtB == ZbOyBYhxGb){wbhAyneLku = true;}
      if(NHVuaGXoaJ == MKBHcirjpr){SioVjDUXZD = true;}
      else if(MKBHcirjpr == NHVuaGXoaJ){gAVXENeAls = true;}
      if(dcfcbLtsJX == ORnyFXoSoP){jYRkBqIgWU = true;}
      else if(ORnyFXoSoP == dcfcbLtsJX){snfXiYKpjp = true;}
      if(slBPwiJbfh == DYbSGQwqtH){uHELiLuElu = true;}
      else if(DYbSGQwqtH == slBPwiJbfh){RFyDeKEHPc = true;}
      if(qgsssNAywF == yftQmbWyNU){ZQkwwaFVYH = true;}
      else if(yftQmbWyNU == qgsssNAywF){zGFBgnNgpf = true;}
      if(fCYMywwkVo == KJURCpGBzk){DQoOOUWKzp = true;}
      else if(KJURCpGBzk == fCYMywwkVo){IBudrHuKRZ = true;}
      if(enmhibGFyq == uZreynhhBM){BlODYLedmX = true;}
      if(wuJrRwjljN == lApisiWWLc){IPkddTmVna = true;}
      if(LwwDrqMnRz == AqFTFtDJPP){lmHOQeRpxO = true;}
      while(uZreynhhBM == enmhibGFyq){cnaucbCSst = true;}
      while(lApisiWWLc == lApisiWWLc){IWjIslidqt = true;}
      while(AqFTFtDJPP == AqFTFtDJPP){KdzDgQJTKW = true;}
      if(KuqILYnPSz == true){KuqILYnPSz = false;}
      if(sowAhJTlYT == true){sowAhJTlYT = false;}
      if(SioVjDUXZD == true){SioVjDUXZD = false;}
      if(jYRkBqIgWU == true){jYRkBqIgWU = false;}
      if(uHELiLuElu == true){uHELiLuElu = false;}
      if(ZQkwwaFVYH == true){ZQkwwaFVYH = false;}
      if(DQoOOUWKzp == true){DQoOOUWKzp = false;}
      if(BlODYLedmX == true){BlODYLedmX = false;}
      if(IPkddTmVna == true){IPkddTmVna = false;}
      if(lmHOQeRpxO == true){lmHOQeRpxO = false;}
      if(ojFutmBhXk == true){ojFutmBhXk = false;}
      if(wbhAyneLku == true){wbhAyneLku = false;}
      if(gAVXENeAls == true){gAVXENeAls = false;}
      if(snfXiYKpjp == true){snfXiYKpjp = false;}
      if(RFyDeKEHPc == true){RFyDeKEHPc = false;}
      if(zGFBgnNgpf == true){zGFBgnNgpf = false;}
      if(IBudrHuKRZ == true){IBudrHuKRZ = false;}
      if(cnaucbCSst == true){cnaucbCSst = false;}
      if(IWjIslidqt == true){IWjIslidqt = false;}
      if(KdzDgQJTKW == true){KdzDgQJTKW = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NMVBOEPFNZ
{ 
  void izWaznlSOa()
  { 
      bool EsuktifVxM = false;
      bool UDHcLWIdyT = false;
      bool zfAxtTIzHl = false;
      bool yOVYiIxfgZ = false;
      bool oIiqGUKxVh = false;
      bool UdCiGXsyqa = false;
      bool ZgOLOFghsC = false;
      bool YcxXepwIpY = false;
      bool ILcYRTMmSK = false;
      bool PyoGAHsXcA = false;
      bool mbDTIqgdDP = false;
      bool ajHRROEost = false;
      bool rtPhlZmqXW = false;
      bool wYfolgHIPZ = false;
      bool lijCifqWnk = false;
      bool DDehHsMVgh = false;
      bool EoFgTCERfk = false;
      bool HuZTHJJNyG = false;
      bool iQDmSrsxgr = false;
      bool TlahVxcCif = false;
      string oHtFllFscJ;
      string NKoemxfjqu;
      string AWZCyVrREw;
      string pPsJqHTMoO;
      string dcuzmbZnDs;
      string xpnBtRhqHQ;
      string tmnfSxRfmf;
      string dStbRWbZOV;
      string uENLzzarOZ;
      string ZbFYUpoOfp;
      string JsAWPdwMiW;
      string pHckUoYbgT;
      string VYQNGBNVGa;
      string hMtXMBEpuU;
      string DOIqiRyUEf;
      string RMhMTkclQN;
      string PZFIodNEYS;
      string khgLPHPYen;
      string CsRaJUegEa;
      string YRZhEcIACF;
      if(oHtFllFscJ == JsAWPdwMiW){EsuktifVxM = true;}
      else if(JsAWPdwMiW == oHtFllFscJ){mbDTIqgdDP = true;}
      if(NKoemxfjqu == pHckUoYbgT){UDHcLWIdyT = true;}
      else if(pHckUoYbgT == NKoemxfjqu){ajHRROEost = true;}
      if(AWZCyVrREw == VYQNGBNVGa){zfAxtTIzHl = true;}
      else if(VYQNGBNVGa == AWZCyVrREw){rtPhlZmqXW = true;}
      if(pPsJqHTMoO == hMtXMBEpuU){yOVYiIxfgZ = true;}
      else if(hMtXMBEpuU == pPsJqHTMoO){wYfolgHIPZ = true;}
      if(dcuzmbZnDs == DOIqiRyUEf){oIiqGUKxVh = true;}
      else if(DOIqiRyUEf == dcuzmbZnDs){lijCifqWnk = true;}
      if(xpnBtRhqHQ == RMhMTkclQN){UdCiGXsyqa = true;}
      else if(RMhMTkclQN == xpnBtRhqHQ){DDehHsMVgh = true;}
      if(tmnfSxRfmf == PZFIodNEYS){ZgOLOFghsC = true;}
      else if(PZFIodNEYS == tmnfSxRfmf){EoFgTCERfk = true;}
      if(dStbRWbZOV == khgLPHPYen){YcxXepwIpY = true;}
      if(uENLzzarOZ == CsRaJUegEa){ILcYRTMmSK = true;}
      if(ZbFYUpoOfp == YRZhEcIACF){PyoGAHsXcA = true;}
      while(khgLPHPYen == dStbRWbZOV){HuZTHJJNyG = true;}
      while(CsRaJUegEa == CsRaJUegEa){iQDmSrsxgr = true;}
      while(YRZhEcIACF == YRZhEcIACF){TlahVxcCif = true;}
      if(EsuktifVxM == true){EsuktifVxM = false;}
      if(UDHcLWIdyT == true){UDHcLWIdyT = false;}
      if(zfAxtTIzHl == true){zfAxtTIzHl = false;}
      if(yOVYiIxfgZ == true){yOVYiIxfgZ = false;}
      if(oIiqGUKxVh == true){oIiqGUKxVh = false;}
      if(UdCiGXsyqa == true){UdCiGXsyqa = false;}
      if(ZgOLOFghsC == true){ZgOLOFghsC = false;}
      if(YcxXepwIpY == true){YcxXepwIpY = false;}
      if(ILcYRTMmSK == true){ILcYRTMmSK = false;}
      if(PyoGAHsXcA == true){PyoGAHsXcA = false;}
      if(mbDTIqgdDP == true){mbDTIqgdDP = false;}
      if(ajHRROEost == true){ajHRROEost = false;}
      if(rtPhlZmqXW == true){rtPhlZmqXW = false;}
      if(wYfolgHIPZ == true){wYfolgHIPZ = false;}
      if(lijCifqWnk == true){lijCifqWnk = false;}
      if(DDehHsMVgh == true){DDehHsMVgh = false;}
      if(EoFgTCERfk == true){EoFgTCERfk = false;}
      if(HuZTHJJNyG == true){HuZTHJJNyG = false;}
      if(iQDmSrsxgr == true){iQDmSrsxgr = false;}
      if(TlahVxcCif == true){TlahVxcCif = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class GOUIHUHZNN
{ 
  void PItxnbJBOA()
  { 
      bool fVGnyheUwE = false;
      bool dOKqOERkHV = false;
      bool eIuLuuRkyo = false;
      bool xmwAiOCIXk = false;
      bool VexFclYuPl = false;
      bool nQDCiWZurC = false;
      bool QsVHGqRKIA = false;
      bool BIMWzzLphJ = false;
      bool WVVLBgjtQL = false;
      bool oGPUTcIpyl = false;
      bool TOmOkeDfDj = false;
      bool LjMiftWKGZ = false;
      bool HXswLurqCW = false;
      bool qVqwpHiGaV = false;
      bool cDqgAEsgoZ = false;
      bool FyiYdwUpDp = false;
      bool mLdfsprQrX = false;
      bool KQijxEfMwG = false;
      bool BKCcRrlSRs = false;
      bool lrDkhQYKnJ = false;
      string JRtWYxSrzw;
      string XzxWZoqdab;
      string OIjFTKOWHG;
      string NytThFifeD;
      string iaupiTbWYR;
      string GwfZGBhoCO;
      string hwkrhYFcoU;
      string FLyWrmMsWz;
      string hrxpzRRTNg;
      string WtcQUlpDHJ;
      string OyhDLuxJgS;
      string nnSDfpyusP;
      string GkPHxLhhBG;
      string wJlMkjKtdD;
      string UIltrhaYje;
      string muJqCciZGz;
      string MOHaWdCFSw;
      string fEQouyyUOO;
      string jIxWSBztJH;
      string AjmepFBLXO;
      if(JRtWYxSrzw == OyhDLuxJgS){fVGnyheUwE = true;}
      else if(OyhDLuxJgS == JRtWYxSrzw){TOmOkeDfDj = true;}
      if(XzxWZoqdab == nnSDfpyusP){dOKqOERkHV = true;}
      else if(nnSDfpyusP == XzxWZoqdab){LjMiftWKGZ = true;}
      if(OIjFTKOWHG == GkPHxLhhBG){eIuLuuRkyo = true;}
      else if(GkPHxLhhBG == OIjFTKOWHG){HXswLurqCW = true;}
      if(NytThFifeD == wJlMkjKtdD){xmwAiOCIXk = true;}
      else if(wJlMkjKtdD == NytThFifeD){qVqwpHiGaV = true;}
      if(iaupiTbWYR == UIltrhaYje){VexFclYuPl = true;}
      else if(UIltrhaYje == iaupiTbWYR){cDqgAEsgoZ = true;}
      if(GwfZGBhoCO == muJqCciZGz){nQDCiWZurC = true;}
      else if(muJqCciZGz == GwfZGBhoCO){FyiYdwUpDp = true;}
      if(hwkrhYFcoU == MOHaWdCFSw){QsVHGqRKIA = true;}
      else if(MOHaWdCFSw == hwkrhYFcoU){mLdfsprQrX = true;}
      if(FLyWrmMsWz == fEQouyyUOO){BIMWzzLphJ = true;}
      if(hrxpzRRTNg == jIxWSBztJH){WVVLBgjtQL = true;}
      if(WtcQUlpDHJ == AjmepFBLXO){oGPUTcIpyl = true;}
      while(fEQouyyUOO == FLyWrmMsWz){KQijxEfMwG = true;}
      while(jIxWSBztJH == jIxWSBztJH){BKCcRrlSRs = true;}
      while(AjmepFBLXO == AjmepFBLXO){lrDkhQYKnJ = true;}
      if(fVGnyheUwE == true){fVGnyheUwE = false;}
      if(dOKqOERkHV == true){dOKqOERkHV = false;}
      if(eIuLuuRkyo == true){eIuLuuRkyo = false;}
      if(xmwAiOCIXk == true){xmwAiOCIXk = false;}
      if(VexFclYuPl == true){VexFclYuPl = false;}
      if(nQDCiWZurC == true){nQDCiWZurC = false;}
      if(QsVHGqRKIA == true){QsVHGqRKIA = false;}
      if(BIMWzzLphJ == true){BIMWzzLphJ = false;}
      if(WVVLBgjtQL == true){WVVLBgjtQL = false;}
      if(oGPUTcIpyl == true){oGPUTcIpyl = false;}
      if(TOmOkeDfDj == true){TOmOkeDfDj = false;}
      if(LjMiftWKGZ == true){LjMiftWKGZ = false;}
      if(HXswLurqCW == true){HXswLurqCW = false;}
      if(qVqwpHiGaV == true){qVqwpHiGaV = false;}
      if(cDqgAEsgoZ == true){cDqgAEsgoZ = false;}
      if(FyiYdwUpDp == true){FyiYdwUpDp = false;}
      if(mLdfsprQrX == true){mLdfsprQrX = false;}
      if(KQijxEfMwG == true){KQijxEfMwG = false;}
      if(BKCcRrlSRs == true){BKCcRrlSRs = false;}
      if(lrDkhQYKnJ == true){lrDkhQYKnJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KEJJCBPOMN
{ 
  void hMsLwYtDol()
  { 
      bool OxHcORlVFi = false;
      bool qTBGspdxET = false;
      bool cdSYjfVdFH = false;
      bool dMgxOkfCwm = false;
      bool ZHzIslGlkn = false;
      bool TPuhXjeiri = false;
      bool wHQxEelErH = false;
      bool KbARsUdJfy = false;
      bool ZirQmOQzYe = false;
      bool GxKNpRqCEN = false;
      bool zBtcdrOdoD = false;
      bool FUCtJNOzJo = false;
      bool NmfNBNRlba = false;
      bool dbERyXoPmu = false;
      bool zmJVZktPEc = false;
      bool CqrOALVgbP = false;
      bool acBNYcPkwk = false;
      bool XzMOoZdJmP = false;
      bool SBhyXmjpRl = false;
      bool BHmWUWkwww = false;
      string FrauTiFGsa;
      string bqJhRtLUTP;
      string LbzsamYYSE;
      string tYMWbqSlBI;
      string FwpHoHHLkt;
      string JchGCcrgTg;
      string JfmogCdlFZ;
      string bYTLKPlrET;
      string fNnYtRqGUz;
      string ykSUyYIBal;
      string cUFKSkIVul;
      string qAcABurOtO;
      string LXRFaqGzNl;
      string GjQRpKHcFJ;
      string IUKhHyHRTC;
      string gtGLZFDzjb;
      string dJIJfGjKKq;
      string nOIjGqNiyc;
      string ineLEMdFrk;
      string GHsdeyMACB;
      if(FrauTiFGsa == cUFKSkIVul){OxHcORlVFi = true;}
      else if(cUFKSkIVul == FrauTiFGsa){zBtcdrOdoD = true;}
      if(bqJhRtLUTP == qAcABurOtO){qTBGspdxET = true;}
      else if(qAcABurOtO == bqJhRtLUTP){FUCtJNOzJo = true;}
      if(LbzsamYYSE == LXRFaqGzNl){cdSYjfVdFH = true;}
      else if(LXRFaqGzNl == LbzsamYYSE){NmfNBNRlba = true;}
      if(tYMWbqSlBI == GjQRpKHcFJ){dMgxOkfCwm = true;}
      else if(GjQRpKHcFJ == tYMWbqSlBI){dbERyXoPmu = true;}
      if(FwpHoHHLkt == IUKhHyHRTC){ZHzIslGlkn = true;}
      else if(IUKhHyHRTC == FwpHoHHLkt){zmJVZktPEc = true;}
      if(JchGCcrgTg == gtGLZFDzjb){TPuhXjeiri = true;}
      else if(gtGLZFDzjb == JchGCcrgTg){CqrOALVgbP = true;}
      if(JfmogCdlFZ == dJIJfGjKKq){wHQxEelErH = true;}
      else if(dJIJfGjKKq == JfmogCdlFZ){acBNYcPkwk = true;}
      if(bYTLKPlrET == nOIjGqNiyc){KbARsUdJfy = true;}
      if(fNnYtRqGUz == ineLEMdFrk){ZirQmOQzYe = true;}
      if(ykSUyYIBal == GHsdeyMACB){GxKNpRqCEN = true;}
      while(nOIjGqNiyc == bYTLKPlrET){XzMOoZdJmP = true;}
      while(ineLEMdFrk == ineLEMdFrk){SBhyXmjpRl = true;}
      while(GHsdeyMACB == GHsdeyMACB){BHmWUWkwww = true;}
      if(OxHcORlVFi == true){OxHcORlVFi = false;}
      if(qTBGspdxET == true){qTBGspdxET = false;}
      if(cdSYjfVdFH == true){cdSYjfVdFH = false;}
      if(dMgxOkfCwm == true){dMgxOkfCwm = false;}
      if(ZHzIslGlkn == true){ZHzIslGlkn = false;}
      if(TPuhXjeiri == true){TPuhXjeiri = false;}
      if(wHQxEelErH == true){wHQxEelErH = false;}
      if(KbARsUdJfy == true){KbARsUdJfy = false;}
      if(ZirQmOQzYe == true){ZirQmOQzYe = false;}
      if(GxKNpRqCEN == true){GxKNpRqCEN = false;}
      if(zBtcdrOdoD == true){zBtcdrOdoD = false;}
      if(FUCtJNOzJo == true){FUCtJNOzJo = false;}
      if(NmfNBNRlba == true){NmfNBNRlba = false;}
      if(dbERyXoPmu == true){dbERyXoPmu = false;}
      if(zmJVZktPEc == true){zmJVZktPEc = false;}
      if(CqrOALVgbP == true){CqrOALVgbP = false;}
      if(acBNYcPkwk == true){acBNYcPkwk = false;}
      if(XzMOoZdJmP == true){XzMOoZdJmP = false;}
      if(SBhyXmjpRl == true){SBhyXmjpRl = false;}
      if(BHmWUWkwww == true){BHmWUWkwww = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SOVKGSFSBW
{ 
  void kSHqXhLQRG()
  { 
      bool VrCALMWobb = false;
      bool WZmzAVXTuH = false;
      bool NygOOaUOsT = false;
      bool qmwaDTUxXd = false;
      bool OoAIaEVXuf = false;
      bool HSDQeWKmlk = false;
      bool rHwHjidMuf = false;
      bool BauQsXyYnK = false;
      bool EwTNMYjeug = false;
      bool JgKmkGsrXq = false;
      bool aJcOefWlXG = false;
      bool XlzbfwnitL = false;
      bool yyQOWTQIel = false;
      bool eFAwitTFhf = false;
      bool CxHoIWgiGo = false;
      bool SKyMsbsNqK = false;
      bool PNGkaRlDrl = false;
      bool biANJApaFA = false;
      bool lhBpSUHnDH = false;
      bool BFNXQnqWNp = false;
      string kmKHeRNWqA;
      string jditjaAoCN;
      string ikbmEGoiLY;
      string hnoPtloikY;
      string XNstWTAMqU;
      string xSNWjzLXCY;
      string ceLRRuoSAx;
      string lREcoVqgQk;
      string GbqqaPaSft;
      string aQfMwsCbca;
      string egMtheNjhN;
      string IdHRRAnTqB;
      string UWmFVVlrfw;
      string TPDpFYPwIu;
      string BpjRlwJOtw;
      string EhjbFItGYM;
      string sVElssaQLs;
      string jrKuzdlUTY;
      string LWxjqlebjx;
      string dJXwCflyYs;
      if(kmKHeRNWqA == egMtheNjhN){VrCALMWobb = true;}
      else if(egMtheNjhN == kmKHeRNWqA){aJcOefWlXG = true;}
      if(jditjaAoCN == IdHRRAnTqB){WZmzAVXTuH = true;}
      else if(IdHRRAnTqB == jditjaAoCN){XlzbfwnitL = true;}
      if(ikbmEGoiLY == UWmFVVlrfw){NygOOaUOsT = true;}
      else if(UWmFVVlrfw == ikbmEGoiLY){yyQOWTQIel = true;}
      if(hnoPtloikY == TPDpFYPwIu){qmwaDTUxXd = true;}
      else if(TPDpFYPwIu == hnoPtloikY){eFAwitTFhf = true;}
      if(XNstWTAMqU == BpjRlwJOtw){OoAIaEVXuf = true;}
      else if(BpjRlwJOtw == XNstWTAMqU){CxHoIWgiGo = true;}
      if(xSNWjzLXCY == EhjbFItGYM){HSDQeWKmlk = true;}
      else if(EhjbFItGYM == xSNWjzLXCY){SKyMsbsNqK = true;}
      if(ceLRRuoSAx == sVElssaQLs){rHwHjidMuf = true;}
      else if(sVElssaQLs == ceLRRuoSAx){PNGkaRlDrl = true;}
      if(lREcoVqgQk == jrKuzdlUTY){BauQsXyYnK = true;}
      if(GbqqaPaSft == LWxjqlebjx){EwTNMYjeug = true;}
      if(aQfMwsCbca == dJXwCflyYs){JgKmkGsrXq = true;}
      while(jrKuzdlUTY == lREcoVqgQk){biANJApaFA = true;}
      while(LWxjqlebjx == LWxjqlebjx){lhBpSUHnDH = true;}
      while(dJXwCflyYs == dJXwCflyYs){BFNXQnqWNp = true;}
      if(VrCALMWobb == true){VrCALMWobb = false;}
      if(WZmzAVXTuH == true){WZmzAVXTuH = false;}
      if(NygOOaUOsT == true){NygOOaUOsT = false;}
      if(qmwaDTUxXd == true){qmwaDTUxXd = false;}
      if(OoAIaEVXuf == true){OoAIaEVXuf = false;}
      if(HSDQeWKmlk == true){HSDQeWKmlk = false;}
      if(rHwHjidMuf == true){rHwHjidMuf = false;}
      if(BauQsXyYnK == true){BauQsXyYnK = false;}
      if(EwTNMYjeug == true){EwTNMYjeug = false;}
      if(JgKmkGsrXq == true){JgKmkGsrXq = false;}
      if(aJcOefWlXG == true){aJcOefWlXG = false;}
      if(XlzbfwnitL == true){XlzbfwnitL = false;}
      if(yyQOWTQIel == true){yyQOWTQIel = false;}
      if(eFAwitTFhf == true){eFAwitTFhf = false;}
      if(CxHoIWgiGo == true){CxHoIWgiGo = false;}
      if(SKyMsbsNqK == true){SKyMsbsNqK = false;}
      if(PNGkaRlDrl == true){PNGkaRlDrl = false;}
      if(biANJApaFA == true){biANJApaFA = false;}
      if(lhBpSUHnDH == true){lhBpSUHnDH = false;}
      if(BFNXQnqWNp == true){BFNXQnqWNp = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CIRXPZWVSZ
{ 
  void jGaePMVmCA()
  { 
      bool napHMbWqWm = false;
      bool EjoSmgsoWH = false;
      bool RfYPOBDRDE = false;
      bool GRSMpiLmgS = false;
      bool YGPHMwIAdu = false;
      bool WAoLFICarL = false;
      bool YEnSglRbbD = false;
      bool VGIEiKPRDS = false;
      bool rCmIgFcGHh = false;
      bool gjtUtTSxaZ = false;
      bool dgYZYQrkct = false;
      bool rjXZSVQeka = false;
      bool JLTsVkLmKO = false;
      bool qSqcnLdNiQ = false;
      bool NqcMoxMTCX = false;
      bool CnLzQSNeEn = false;
      bool EFZODZrukG = false;
      bool rDuUNRhGDY = false;
      bool YqjmLDHrLV = false;
      bool yyJuMHROJP = false;
      string eLEOhsSpPh;
      string TiPoohKfGJ;
      string RJTUmAGcaF;
      string WmietxefhT;
      string bQYaksQobH;
      string lhasEVqibg;
      string loPKLgAjtr;
      string CdxmgAjsjt;
      string iQBSSnLpaX;
      string FuEAgmAOOE;
      string sfujEIjnLD;
      string lxzgyUnwqX;
      string UJJVlGYxFt;
      string zVeeADVQOK;
      string DXeWlHwWCL;
      string zEWqjiWYxx;
      string imhRDPSUgu;
      string zWhfTwelje;
      string KBtstuymuQ;
      string fxTQbPoMTW;
      if(eLEOhsSpPh == sfujEIjnLD){napHMbWqWm = true;}
      else if(sfujEIjnLD == eLEOhsSpPh){dgYZYQrkct = true;}
      if(TiPoohKfGJ == lxzgyUnwqX){EjoSmgsoWH = true;}
      else if(lxzgyUnwqX == TiPoohKfGJ){rjXZSVQeka = true;}
      if(RJTUmAGcaF == UJJVlGYxFt){RfYPOBDRDE = true;}
      else if(UJJVlGYxFt == RJTUmAGcaF){JLTsVkLmKO = true;}
      if(WmietxefhT == zVeeADVQOK){GRSMpiLmgS = true;}
      else if(zVeeADVQOK == WmietxefhT){qSqcnLdNiQ = true;}
      if(bQYaksQobH == DXeWlHwWCL){YGPHMwIAdu = true;}
      else if(DXeWlHwWCL == bQYaksQobH){NqcMoxMTCX = true;}
      if(lhasEVqibg == zEWqjiWYxx){WAoLFICarL = true;}
      else if(zEWqjiWYxx == lhasEVqibg){CnLzQSNeEn = true;}
      if(loPKLgAjtr == imhRDPSUgu){YEnSglRbbD = true;}
      else if(imhRDPSUgu == loPKLgAjtr){EFZODZrukG = true;}
      if(CdxmgAjsjt == zWhfTwelje){VGIEiKPRDS = true;}
      if(iQBSSnLpaX == KBtstuymuQ){rCmIgFcGHh = true;}
      if(FuEAgmAOOE == fxTQbPoMTW){gjtUtTSxaZ = true;}
      while(zWhfTwelje == CdxmgAjsjt){rDuUNRhGDY = true;}
      while(KBtstuymuQ == KBtstuymuQ){YqjmLDHrLV = true;}
      while(fxTQbPoMTW == fxTQbPoMTW){yyJuMHROJP = true;}
      if(napHMbWqWm == true){napHMbWqWm = false;}
      if(EjoSmgsoWH == true){EjoSmgsoWH = false;}
      if(RfYPOBDRDE == true){RfYPOBDRDE = false;}
      if(GRSMpiLmgS == true){GRSMpiLmgS = false;}
      if(YGPHMwIAdu == true){YGPHMwIAdu = false;}
      if(WAoLFICarL == true){WAoLFICarL = false;}
      if(YEnSglRbbD == true){YEnSglRbbD = false;}
      if(VGIEiKPRDS == true){VGIEiKPRDS = false;}
      if(rCmIgFcGHh == true){rCmIgFcGHh = false;}
      if(gjtUtTSxaZ == true){gjtUtTSxaZ = false;}
      if(dgYZYQrkct == true){dgYZYQrkct = false;}
      if(rjXZSVQeka == true){rjXZSVQeka = false;}
      if(JLTsVkLmKO == true){JLTsVkLmKO = false;}
      if(qSqcnLdNiQ == true){qSqcnLdNiQ = false;}
      if(NqcMoxMTCX == true){NqcMoxMTCX = false;}
      if(CnLzQSNeEn == true){CnLzQSNeEn = false;}
      if(EFZODZrukG == true){EFZODZrukG = false;}
      if(rDuUNRhGDY == true){rDuUNRhGDY = false;}
      if(YqjmLDHrLV == true){YqjmLDHrLV = false;}
      if(yyJuMHROJP == true){yyJuMHROJP = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LXQOMHFFGT
{ 
  void aLMpjGgEcM()
  { 
      bool bPwJOBVqWi = false;
      bool JopZOXXrbI = false;
      bool gTwwoPHjYF = false;
      bool GEfapMmabx = false;
      bool PrWnXEPZZx = false;
      bool GspusHSzTz = false;
      bool skRQcEJPiC = false;
      bool jwPdsoxIYk = false;
      bool zhKbzMNjgi = false;
      bool tnHcDdaFLS = false;
      bool tApXMKdMCE = false;
      bool KJZhzLaoRQ = false;
      bool pZhAIunmZO = false;
      bool OqxGqzqlOo = false;
      bool hBgbXjNYcr = false;
      bool DkmtbTRoOR = false;
      bool VqlnpSMTqA = false;
      bool TnZchosGcY = false;
      bool gOJgKcDCdW = false;
      bool gBXxSqPslE = false;
      string PqexayzpBb;
      string YdsLIdpCyY;
      string YBzWytrFCj;
      string hdjjpwNPcN;
      string EoYVioRGMT;
      string qLwFKNrIzf;
      string GtnQsttIUL;
      string QZZjUiccnI;
      string ckshsWgdZb;
      string EEQwHKBIcn;
      string uIkACQkMka;
      string kOUqDVAFWV;
      string MPisHLJccJ;
      string gUAzMTzrfc;
      string lUsyqojZsk;
      string JukGafZrrp;
      string HHiatPNUdj;
      string WIZtjrWjBR;
      string bjjqzkisXg;
      string tGAoUdlREa;
      if(PqexayzpBb == uIkACQkMka){bPwJOBVqWi = true;}
      else if(uIkACQkMka == PqexayzpBb){tApXMKdMCE = true;}
      if(YdsLIdpCyY == kOUqDVAFWV){JopZOXXrbI = true;}
      else if(kOUqDVAFWV == YdsLIdpCyY){KJZhzLaoRQ = true;}
      if(YBzWytrFCj == MPisHLJccJ){gTwwoPHjYF = true;}
      else if(MPisHLJccJ == YBzWytrFCj){pZhAIunmZO = true;}
      if(hdjjpwNPcN == gUAzMTzrfc){GEfapMmabx = true;}
      else if(gUAzMTzrfc == hdjjpwNPcN){OqxGqzqlOo = true;}
      if(EoYVioRGMT == lUsyqojZsk){PrWnXEPZZx = true;}
      else if(lUsyqojZsk == EoYVioRGMT){hBgbXjNYcr = true;}
      if(qLwFKNrIzf == JukGafZrrp){GspusHSzTz = true;}
      else if(JukGafZrrp == qLwFKNrIzf){DkmtbTRoOR = true;}
      if(GtnQsttIUL == HHiatPNUdj){skRQcEJPiC = true;}
      else if(HHiatPNUdj == GtnQsttIUL){VqlnpSMTqA = true;}
      if(QZZjUiccnI == WIZtjrWjBR){jwPdsoxIYk = true;}
      if(ckshsWgdZb == bjjqzkisXg){zhKbzMNjgi = true;}
      if(EEQwHKBIcn == tGAoUdlREa){tnHcDdaFLS = true;}
      while(WIZtjrWjBR == QZZjUiccnI){TnZchosGcY = true;}
      while(bjjqzkisXg == bjjqzkisXg){gOJgKcDCdW = true;}
      while(tGAoUdlREa == tGAoUdlREa){gBXxSqPslE = true;}
      if(bPwJOBVqWi == true){bPwJOBVqWi = false;}
      if(JopZOXXrbI == true){JopZOXXrbI = false;}
      if(gTwwoPHjYF == true){gTwwoPHjYF = false;}
      if(GEfapMmabx == true){GEfapMmabx = false;}
      if(PrWnXEPZZx == true){PrWnXEPZZx = false;}
      if(GspusHSzTz == true){GspusHSzTz = false;}
      if(skRQcEJPiC == true){skRQcEJPiC = false;}
      if(jwPdsoxIYk == true){jwPdsoxIYk = false;}
      if(zhKbzMNjgi == true){zhKbzMNjgi = false;}
      if(tnHcDdaFLS == true){tnHcDdaFLS = false;}
      if(tApXMKdMCE == true){tApXMKdMCE = false;}
      if(KJZhzLaoRQ == true){KJZhzLaoRQ = false;}
      if(pZhAIunmZO == true){pZhAIunmZO = false;}
      if(OqxGqzqlOo == true){OqxGqzqlOo = false;}
      if(hBgbXjNYcr == true){hBgbXjNYcr = false;}
      if(DkmtbTRoOR == true){DkmtbTRoOR = false;}
      if(VqlnpSMTqA == true){VqlnpSMTqA = false;}
      if(TnZchosGcY == true){TnZchosGcY = false;}
      if(gOJgKcDCdW == true){gOJgKcDCdW = false;}
      if(gBXxSqPslE == true){gBXxSqPslE = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LLXOBMECRV
{ 
  void BcMuWnICJB()
  { 
      bool DOaYOiqCtR = false;
      bool JlNZEQQxiW = false;
      bool htyorUJSKD = false;
      bool NqQptyyGUK = false;
      bool riwHneyLnY = false;
      bool JgklfJQkfs = false;
      bool YYqpESUXfG = false;
      bool RKbXdzypBN = false;
      bool VydksgGeJj = false;
      bool hMZblTqFDV = false;
      bool JafhGzEMHg = false;
      bool LZjRWZuGwX = false;
      bool LoNuXFjAkK = false;
      bool HrmfEuRuet = false;
      bool bsXcXIbIEL = false;
      bool XkIeJTNjKA = false;
      bool PEbwYSlENb = false;
      bool HzaCwSRkWO = false;
      bool cbuYlWjHVj = false;
      bool ghkdeXRaDF = false;
      string EwwjyLiKfH;
      string LSafJFhRDW;
      string BhDoghMHDt;
      string XhLYIhFYjK;
      string MjQLqjDclV;
      string lqHMMVsHDQ;
      string yaGlidGBXb;
      string AIgsKRQrxw;
      string djUhHgZzLl;
      string oyUTLkcebN;
      string cBEKUcJOCu;
      string EAJGlOGYdC;
      string DHpllCnNgY;
      string nlPjSbfJMe;
      string WLwPgnnOhx;
      string eDBZJULSXS;
      string owgcHSjYmJ;
      string hwMzeqKZEy;
      string NYejtjkpZy;
      string QMlsoadTIO;
      if(EwwjyLiKfH == cBEKUcJOCu){DOaYOiqCtR = true;}
      else if(cBEKUcJOCu == EwwjyLiKfH){JafhGzEMHg = true;}
      if(LSafJFhRDW == EAJGlOGYdC){JlNZEQQxiW = true;}
      else if(EAJGlOGYdC == LSafJFhRDW){LZjRWZuGwX = true;}
      if(BhDoghMHDt == DHpllCnNgY){htyorUJSKD = true;}
      else if(DHpllCnNgY == BhDoghMHDt){LoNuXFjAkK = true;}
      if(XhLYIhFYjK == nlPjSbfJMe){NqQptyyGUK = true;}
      else if(nlPjSbfJMe == XhLYIhFYjK){HrmfEuRuet = true;}
      if(MjQLqjDclV == WLwPgnnOhx){riwHneyLnY = true;}
      else if(WLwPgnnOhx == MjQLqjDclV){bsXcXIbIEL = true;}
      if(lqHMMVsHDQ == eDBZJULSXS){JgklfJQkfs = true;}
      else if(eDBZJULSXS == lqHMMVsHDQ){XkIeJTNjKA = true;}
      if(yaGlidGBXb == owgcHSjYmJ){YYqpESUXfG = true;}
      else if(owgcHSjYmJ == yaGlidGBXb){PEbwYSlENb = true;}
      if(AIgsKRQrxw == hwMzeqKZEy){RKbXdzypBN = true;}
      if(djUhHgZzLl == NYejtjkpZy){VydksgGeJj = true;}
      if(oyUTLkcebN == QMlsoadTIO){hMZblTqFDV = true;}
      while(hwMzeqKZEy == AIgsKRQrxw){HzaCwSRkWO = true;}
      while(NYejtjkpZy == NYejtjkpZy){cbuYlWjHVj = true;}
      while(QMlsoadTIO == QMlsoadTIO){ghkdeXRaDF = true;}
      if(DOaYOiqCtR == true){DOaYOiqCtR = false;}
      if(JlNZEQQxiW == true){JlNZEQQxiW = false;}
      if(htyorUJSKD == true){htyorUJSKD = false;}
      if(NqQptyyGUK == true){NqQptyyGUK = false;}
      if(riwHneyLnY == true){riwHneyLnY = false;}
      if(JgklfJQkfs == true){JgklfJQkfs = false;}
      if(YYqpESUXfG == true){YYqpESUXfG = false;}
      if(RKbXdzypBN == true){RKbXdzypBN = false;}
      if(VydksgGeJj == true){VydksgGeJj = false;}
      if(hMZblTqFDV == true){hMZblTqFDV = false;}
      if(JafhGzEMHg == true){JafhGzEMHg = false;}
      if(LZjRWZuGwX == true){LZjRWZuGwX = false;}
      if(LoNuXFjAkK == true){LoNuXFjAkK = false;}
      if(HrmfEuRuet == true){HrmfEuRuet = false;}
      if(bsXcXIbIEL == true){bsXcXIbIEL = false;}
      if(XkIeJTNjKA == true){XkIeJTNjKA = false;}
      if(PEbwYSlENb == true){PEbwYSlENb = false;}
      if(HzaCwSRkWO == true){HzaCwSRkWO = false;}
      if(cbuYlWjHVj == true){cbuYlWjHVj = false;}
      if(ghkdeXRaDF == true){ghkdeXRaDF = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KLJDDWCPBP
{ 
  void fMJodDZCSw()
  { 
      bool JnYLKGqeLg = false;
      bool ALOdQtfGjk = false;
      bool GpRbyfoILO = false;
      bool ukeTHsIFnU = false;
      bool DYmlYfhZcu = false;
      bool whqHgSVwNd = false;
      bool RhTwkPftWW = false;
      bool kZSPhhCeDR = false;
      bool xOfQSNsXmn = false;
      bool XLrJCUFqAI = false;
      bool znDFJWObYz = false;
      bool QprufDBDdX = false;
      bool WIHizYhJMM = false;
      bool bLeuSJKZGu = false;
      bool yQymXgTwdz = false;
      bool TBMbkpfUGj = false;
      bool nFPMWELxcV = false;
      bool rawhZGKTLR = false;
      bool FkfMsazCKb = false;
      bool dpTWGsAlQS = false;
      string UFsdAqTqVH;
      string YIYfyydhSQ;
      string CrwrnaoSCG;
      string yHikZwwFsq;
      string ubLbMmTlKG;
      string HGUIscJBpY;
      string CBoPIQZnFB;
      string BXnbUZfTeA;
      string HCIuzrmZqw;
      string muPToslwSu;
      string NfgIEiRrmS;
      string woVoRzssPQ;
      string dzwIOGHUmb;
      string mjAjmaVzqP;
      string ZtmXAxYaCo;
      string AsfCQNiXtx;
      string axFLfLnkBp;
      string dsCAqjMXIQ;
      string ggRyatEyTZ;
      string rxEsobobrn;
      if(UFsdAqTqVH == NfgIEiRrmS){JnYLKGqeLg = true;}
      else if(NfgIEiRrmS == UFsdAqTqVH){znDFJWObYz = true;}
      if(YIYfyydhSQ == woVoRzssPQ){ALOdQtfGjk = true;}
      else if(woVoRzssPQ == YIYfyydhSQ){QprufDBDdX = true;}
      if(CrwrnaoSCG == dzwIOGHUmb){GpRbyfoILO = true;}
      else if(dzwIOGHUmb == CrwrnaoSCG){WIHizYhJMM = true;}
      if(yHikZwwFsq == mjAjmaVzqP){ukeTHsIFnU = true;}
      else if(mjAjmaVzqP == yHikZwwFsq){bLeuSJKZGu = true;}
      if(ubLbMmTlKG == ZtmXAxYaCo){DYmlYfhZcu = true;}
      else if(ZtmXAxYaCo == ubLbMmTlKG){yQymXgTwdz = true;}
      if(HGUIscJBpY == AsfCQNiXtx){whqHgSVwNd = true;}
      else if(AsfCQNiXtx == HGUIscJBpY){TBMbkpfUGj = true;}
      if(CBoPIQZnFB == axFLfLnkBp){RhTwkPftWW = true;}
      else if(axFLfLnkBp == CBoPIQZnFB){nFPMWELxcV = true;}
      if(BXnbUZfTeA == dsCAqjMXIQ){kZSPhhCeDR = true;}
      if(HCIuzrmZqw == ggRyatEyTZ){xOfQSNsXmn = true;}
      if(muPToslwSu == rxEsobobrn){XLrJCUFqAI = true;}
      while(dsCAqjMXIQ == BXnbUZfTeA){rawhZGKTLR = true;}
      while(ggRyatEyTZ == ggRyatEyTZ){FkfMsazCKb = true;}
      while(rxEsobobrn == rxEsobobrn){dpTWGsAlQS = true;}
      if(JnYLKGqeLg == true){JnYLKGqeLg = false;}
      if(ALOdQtfGjk == true){ALOdQtfGjk = false;}
      if(GpRbyfoILO == true){GpRbyfoILO = false;}
      if(ukeTHsIFnU == true){ukeTHsIFnU = false;}
      if(DYmlYfhZcu == true){DYmlYfhZcu = false;}
      if(whqHgSVwNd == true){whqHgSVwNd = false;}
      if(RhTwkPftWW == true){RhTwkPftWW = false;}
      if(kZSPhhCeDR == true){kZSPhhCeDR = false;}
      if(xOfQSNsXmn == true){xOfQSNsXmn = false;}
      if(XLrJCUFqAI == true){XLrJCUFqAI = false;}
      if(znDFJWObYz == true){znDFJWObYz = false;}
      if(QprufDBDdX == true){QprufDBDdX = false;}
      if(WIHizYhJMM == true){WIHizYhJMM = false;}
      if(bLeuSJKZGu == true){bLeuSJKZGu = false;}
      if(yQymXgTwdz == true){yQymXgTwdz = false;}
      if(TBMbkpfUGj == true){TBMbkpfUGj = false;}
      if(nFPMWELxcV == true){nFPMWELxcV = false;}
      if(rawhZGKTLR == true){rawhZGKTLR = false;}
      if(FkfMsazCKb == true){FkfMsazCKb = false;}
      if(dpTWGsAlQS == true){dpTWGsAlQS = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PMZLJCTTAN
{ 
  void OxbxhQTjwW()
  { 
      bool qhQtLjbKWw = false;
      bool DMBHYkqkps = false;
      bool oxeomQrjOo = false;
      bool yxEiiBboHQ = false;
      bool mNcMlxDFIj = false;
      bool pxotUtCbHu = false;
      bool qrwhxGHSYX = false;
      bool JCcYFbTRCp = false;
      bool MlaeyAHJNo = false;
      bool yAhMYuQuOf = false;
      bool okgjZnUQnr = false;
      bool ZkYRhFQRiw = false;
      bool yyEjZJTPzK = false;
      bool KYCWAbFqEK = false;
      bool fqVUOMaQeT = false;
      bool dzLRgqfxJQ = false;
      bool TEQchBhCQG = false;
      bool AYMYQHciUM = false;
      bool hDkffKNjoI = false;
      bool uHgnpaarmN = false;
      string HyOoIZbbds;
      string VaeBhKoAqW;
      string TdnBkrrhQa;
      string zFWhgoJCTl;
      string mLgTPimfoN;
      string HkLSXcKMDQ;
      string htKDtPDwUs;
      string AmfFjIxTlB;
      string ELrDTNtEKD;
      string ewyDDrAdZz;
      string epLwMTEGhP;
      string fpaBLVCHif;
      string VzlQOEocBq;
      string HSsmWNAwpz;
      string JnVtXnVwrg;
      string pstsdfEthI;
      string dbEqHNxmex;
      string VGRjWhcqsZ;
      string isjTAOXAIp;
      string JFNhUhiemJ;
      if(HyOoIZbbds == epLwMTEGhP){qhQtLjbKWw = true;}
      else if(epLwMTEGhP == HyOoIZbbds){okgjZnUQnr = true;}
      if(VaeBhKoAqW == fpaBLVCHif){DMBHYkqkps = true;}
      else if(fpaBLVCHif == VaeBhKoAqW){ZkYRhFQRiw = true;}
      if(TdnBkrrhQa == VzlQOEocBq){oxeomQrjOo = true;}
      else if(VzlQOEocBq == TdnBkrrhQa){yyEjZJTPzK = true;}
      if(zFWhgoJCTl == HSsmWNAwpz){yxEiiBboHQ = true;}
      else if(HSsmWNAwpz == zFWhgoJCTl){KYCWAbFqEK = true;}
      if(mLgTPimfoN == JnVtXnVwrg){mNcMlxDFIj = true;}
      else if(JnVtXnVwrg == mLgTPimfoN){fqVUOMaQeT = true;}
      if(HkLSXcKMDQ == pstsdfEthI){pxotUtCbHu = true;}
      else if(pstsdfEthI == HkLSXcKMDQ){dzLRgqfxJQ = true;}
      if(htKDtPDwUs == dbEqHNxmex){qrwhxGHSYX = true;}
      else if(dbEqHNxmex == htKDtPDwUs){TEQchBhCQG = true;}
      if(AmfFjIxTlB == VGRjWhcqsZ){JCcYFbTRCp = true;}
      if(ELrDTNtEKD == isjTAOXAIp){MlaeyAHJNo = true;}
      if(ewyDDrAdZz == JFNhUhiemJ){yAhMYuQuOf = true;}
      while(VGRjWhcqsZ == AmfFjIxTlB){AYMYQHciUM = true;}
      while(isjTAOXAIp == isjTAOXAIp){hDkffKNjoI = true;}
      while(JFNhUhiemJ == JFNhUhiemJ){uHgnpaarmN = true;}
      if(qhQtLjbKWw == true){qhQtLjbKWw = false;}
      if(DMBHYkqkps == true){DMBHYkqkps = false;}
      if(oxeomQrjOo == true){oxeomQrjOo = false;}
      if(yxEiiBboHQ == true){yxEiiBboHQ = false;}
      if(mNcMlxDFIj == true){mNcMlxDFIj = false;}
      if(pxotUtCbHu == true){pxotUtCbHu = false;}
      if(qrwhxGHSYX == true){qrwhxGHSYX = false;}
      if(JCcYFbTRCp == true){JCcYFbTRCp = false;}
      if(MlaeyAHJNo == true){MlaeyAHJNo = false;}
      if(yAhMYuQuOf == true){yAhMYuQuOf = false;}
      if(okgjZnUQnr == true){okgjZnUQnr = false;}
      if(ZkYRhFQRiw == true){ZkYRhFQRiw = false;}
      if(yyEjZJTPzK == true){yyEjZJTPzK = false;}
      if(KYCWAbFqEK == true){KYCWAbFqEK = false;}
      if(fqVUOMaQeT == true){fqVUOMaQeT = false;}
      if(dzLRgqfxJQ == true){dzLRgqfxJQ = false;}
      if(TEQchBhCQG == true){TEQchBhCQG = false;}
      if(AYMYQHciUM == true){AYMYQHciUM = false;}
      if(hDkffKNjoI == true){hDkffKNjoI = false;}
      if(uHgnpaarmN == true){uHgnpaarmN = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AMGBUBKNGB
{ 
  void jUudURhUiE()
  { 
      bool VelrMcxDMX = false;
      bool eIZGVTPCqp = false;
      bool hlPDIWldbK = false;
      bool izZcZSIPdk = false;
      bool XWBCfOeQZp = false;
      bool MlXrqiOiip = false;
      bool qTbCoeiwgC = false;
      bool LzYFSKkNWs = false;
      bool rVDkyxaglp = false;
      bool EZgntDuzNA = false;
      bool fhiXeMNSZf = false;
      bool yBLpxVPpEr = false;
      bool DBJpylSjKO = false;
      bool PlzVMJlfGk = false;
      bool jIHGrCNppX = false;
      bool mEtVxdXsZr = false;
      bool tcHKPLBocn = false;
      bool jjiTIutCxu = false;
      bool XbtmaKqdYi = false;
      bool krFNpGBwqf = false;
      string EyhMJzxoIa;
      string RKDmLghzXP;
      string HOfrOdxVxL;
      string iEBDwFaRYy;
      string uDxeQjUDbs;
      string aeyaGpRpxe;
      string CbrwqNIApG;
      string FPZZTDQBae;
      string JstDkEGaxG;
      string mkTtwTLBmD;
      string aTMUnLEXGC;
      string YPOSVIgoLm;
      string GhCjSUwfdA;
      string qPOXDogYPZ;
      string AhTthWVBXm;
      string lWoAwrRbCT;
      string OXsFLIBqOf;
      string aIYlmXNICu;
      string yuIpOUZOaK;
      string XEWuKboOiY;
      if(EyhMJzxoIa == aTMUnLEXGC){VelrMcxDMX = true;}
      else if(aTMUnLEXGC == EyhMJzxoIa){fhiXeMNSZf = true;}
      if(RKDmLghzXP == YPOSVIgoLm){eIZGVTPCqp = true;}
      else if(YPOSVIgoLm == RKDmLghzXP){yBLpxVPpEr = true;}
      if(HOfrOdxVxL == GhCjSUwfdA){hlPDIWldbK = true;}
      else if(GhCjSUwfdA == HOfrOdxVxL){DBJpylSjKO = true;}
      if(iEBDwFaRYy == qPOXDogYPZ){izZcZSIPdk = true;}
      else if(qPOXDogYPZ == iEBDwFaRYy){PlzVMJlfGk = true;}
      if(uDxeQjUDbs == AhTthWVBXm){XWBCfOeQZp = true;}
      else if(AhTthWVBXm == uDxeQjUDbs){jIHGrCNppX = true;}
      if(aeyaGpRpxe == lWoAwrRbCT){MlXrqiOiip = true;}
      else if(lWoAwrRbCT == aeyaGpRpxe){mEtVxdXsZr = true;}
      if(CbrwqNIApG == OXsFLIBqOf){qTbCoeiwgC = true;}
      else if(OXsFLIBqOf == CbrwqNIApG){tcHKPLBocn = true;}
      if(FPZZTDQBae == aIYlmXNICu){LzYFSKkNWs = true;}
      if(JstDkEGaxG == yuIpOUZOaK){rVDkyxaglp = true;}
      if(mkTtwTLBmD == XEWuKboOiY){EZgntDuzNA = true;}
      while(aIYlmXNICu == FPZZTDQBae){jjiTIutCxu = true;}
      while(yuIpOUZOaK == yuIpOUZOaK){XbtmaKqdYi = true;}
      while(XEWuKboOiY == XEWuKboOiY){krFNpGBwqf = true;}
      if(VelrMcxDMX == true){VelrMcxDMX = false;}
      if(eIZGVTPCqp == true){eIZGVTPCqp = false;}
      if(hlPDIWldbK == true){hlPDIWldbK = false;}
      if(izZcZSIPdk == true){izZcZSIPdk = false;}
      if(XWBCfOeQZp == true){XWBCfOeQZp = false;}
      if(MlXrqiOiip == true){MlXrqiOiip = false;}
      if(qTbCoeiwgC == true){qTbCoeiwgC = false;}
      if(LzYFSKkNWs == true){LzYFSKkNWs = false;}
      if(rVDkyxaglp == true){rVDkyxaglp = false;}
      if(EZgntDuzNA == true){EZgntDuzNA = false;}
      if(fhiXeMNSZf == true){fhiXeMNSZf = false;}
      if(yBLpxVPpEr == true){yBLpxVPpEr = false;}
      if(DBJpylSjKO == true){DBJpylSjKO = false;}
      if(PlzVMJlfGk == true){PlzVMJlfGk = false;}
      if(jIHGrCNppX == true){jIHGrCNppX = false;}
      if(mEtVxdXsZr == true){mEtVxdXsZr = false;}
      if(tcHKPLBocn == true){tcHKPLBocn = false;}
      if(jjiTIutCxu == true){jjiTIutCxu = false;}
      if(XbtmaKqdYi == true){XbtmaKqdYi = false;}
      if(krFNpGBwqf == true){krFNpGBwqf = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CCLAZJPBLX
{ 
  void kEuSiUVzjJ()
  { 
      bool ArazaLRGRV = false;
      bool BNjpCFzmrw = false;
      bool szBanxmJWi = false;
      bool YBFbQjlsNI = false;
      bool NVroCqGURG = false;
      bool zfyhhBpzKd = false;
      bool XHihxzIHZR = false;
      bool SFZihWHMYx = false;
      bool pSrQjsCWsO = false;
      bool slhikFlJrC = false;
      bool iyuAmsFPzK = false;
      bool sdaNmEPSyD = false;
      bool xkNKXgWpxc = false;
      bool kxUWOrXrdp = false;
      bool mmXiVPKUwf = false;
      bool pTLnFyjjbF = false;
      bool WPRWZuJEyt = false;
      bool gXnHQJMenI = false;
      bool WCtRlQlfQQ = false;
      bool HmPYTXVBIZ = false;
      string ERDSwpDlSd;
      string KeAllblrJy;
      string KuNqJQBlyt;
      string dupFwhDoSW;
      string rMabQaefll;
      string gFTwzJEMJf;
      string nRnYzPntoD;
      string YxZmFyTlai;
      string kUALMXbsGm;
      string cqfmhaXBbB;
      string mjphMjffqB;
      string oSlZtSQcLa;
      string mtnCahRlEe;
      string ZSJYwAeRyc;
      string ruCEyJFFsP;
      string JuoANrwnHe;
      string RsbMaWIMds;
      string nydiirqiDQ;
      string cMsoUecJKD;
      string EpKTLzSpdb;
      if(ERDSwpDlSd == mjphMjffqB){ArazaLRGRV = true;}
      else if(mjphMjffqB == ERDSwpDlSd){iyuAmsFPzK = true;}
      if(KeAllblrJy == oSlZtSQcLa){BNjpCFzmrw = true;}
      else if(oSlZtSQcLa == KeAllblrJy){sdaNmEPSyD = true;}
      if(KuNqJQBlyt == mtnCahRlEe){szBanxmJWi = true;}
      else if(mtnCahRlEe == KuNqJQBlyt){xkNKXgWpxc = true;}
      if(dupFwhDoSW == ZSJYwAeRyc){YBFbQjlsNI = true;}
      else if(ZSJYwAeRyc == dupFwhDoSW){kxUWOrXrdp = true;}
      if(rMabQaefll == ruCEyJFFsP){NVroCqGURG = true;}
      else if(ruCEyJFFsP == rMabQaefll){mmXiVPKUwf = true;}
      if(gFTwzJEMJf == JuoANrwnHe){zfyhhBpzKd = true;}
      else if(JuoANrwnHe == gFTwzJEMJf){pTLnFyjjbF = true;}
      if(nRnYzPntoD == RsbMaWIMds){XHihxzIHZR = true;}
      else if(RsbMaWIMds == nRnYzPntoD){WPRWZuJEyt = true;}
      if(YxZmFyTlai == nydiirqiDQ){SFZihWHMYx = true;}
      if(kUALMXbsGm == cMsoUecJKD){pSrQjsCWsO = true;}
      if(cqfmhaXBbB == EpKTLzSpdb){slhikFlJrC = true;}
      while(nydiirqiDQ == YxZmFyTlai){gXnHQJMenI = true;}
      while(cMsoUecJKD == cMsoUecJKD){WCtRlQlfQQ = true;}
      while(EpKTLzSpdb == EpKTLzSpdb){HmPYTXVBIZ = true;}
      if(ArazaLRGRV == true){ArazaLRGRV = false;}
      if(BNjpCFzmrw == true){BNjpCFzmrw = false;}
      if(szBanxmJWi == true){szBanxmJWi = false;}
      if(YBFbQjlsNI == true){YBFbQjlsNI = false;}
      if(NVroCqGURG == true){NVroCqGURG = false;}
      if(zfyhhBpzKd == true){zfyhhBpzKd = false;}
      if(XHihxzIHZR == true){XHihxzIHZR = false;}
      if(SFZihWHMYx == true){SFZihWHMYx = false;}
      if(pSrQjsCWsO == true){pSrQjsCWsO = false;}
      if(slhikFlJrC == true){slhikFlJrC = false;}
      if(iyuAmsFPzK == true){iyuAmsFPzK = false;}
      if(sdaNmEPSyD == true){sdaNmEPSyD = false;}
      if(xkNKXgWpxc == true){xkNKXgWpxc = false;}
      if(kxUWOrXrdp == true){kxUWOrXrdp = false;}
      if(mmXiVPKUwf == true){mmXiVPKUwf = false;}
      if(pTLnFyjjbF == true){pTLnFyjjbF = false;}
      if(WPRWZuJEyt == true){WPRWZuJEyt = false;}
      if(gXnHQJMenI == true){gXnHQJMenI = false;}
      if(WCtRlQlfQQ == true){WCtRlQlfQQ = false;}
      if(HmPYTXVBIZ == true){HmPYTXVBIZ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class DZFBKGPDUO
{ 
  void SFeQWFZpHL()
  { 
      bool KBiDJjtpMl = false;
      bool xSfbByfouD = false;
      bool EWgNmVwJxH = false;
      bool aYLqltDmiV = false;
      bool gsxwBZEQzS = false;
      bool XaaLFmUVQm = false;
      bool nksELXGnoH = false;
      bool usFZWKZkMN = false;
      bool RfKeizyAlz = false;
      bool ZhWuhuArjd = false;
      bool TQaqXeVAot = false;
      bool NiICVIaNUM = false;
      bool YFBphMVKra = false;
      bool KgAitAESGE = false;
      bool dxLrkpYTYY = false;
      bool UlWXENOjIK = false;
      bool ILINjfhZIU = false;
      bool DHHQiatJFr = false;
      bool mCkSFXKcdg = false;
      bool lUJwuSoxIE = false;
      string PWfDbfUwIO;
      string KZztDFlHOc;
      string gYQZuDHibU;
      string CgzTSLyGcO;
      string RjLXwoyIER;
      string ycqrKqGFIY;
      string XkTnMdafar;
      string GeeALxMsdF;
      string FzeKVYoGIb;
      string EsxDpfPXKR;
      string NRgMLSyqgN;
      string dNbDTYGJRm;
      string nUWzmNgaJe;
      string gohLqmRRzC;
      string axYuypRtjQ;
      string YFFXcstgYx;
      string kWhCfnfPLf;
      string GoKOLqwbBx;
      string umqsGbjJhy;
      string rkrBREPWPJ;
      if(PWfDbfUwIO == NRgMLSyqgN){KBiDJjtpMl = true;}
      else if(NRgMLSyqgN == PWfDbfUwIO){TQaqXeVAot = true;}
      if(KZztDFlHOc == dNbDTYGJRm){xSfbByfouD = true;}
      else if(dNbDTYGJRm == KZztDFlHOc){NiICVIaNUM = true;}
      if(gYQZuDHibU == nUWzmNgaJe){EWgNmVwJxH = true;}
      else if(nUWzmNgaJe == gYQZuDHibU){YFBphMVKra = true;}
      if(CgzTSLyGcO == gohLqmRRzC){aYLqltDmiV = true;}
      else if(gohLqmRRzC == CgzTSLyGcO){KgAitAESGE = true;}
      if(RjLXwoyIER == axYuypRtjQ){gsxwBZEQzS = true;}
      else if(axYuypRtjQ == RjLXwoyIER){dxLrkpYTYY = true;}
      if(ycqrKqGFIY == YFFXcstgYx){XaaLFmUVQm = true;}
      else if(YFFXcstgYx == ycqrKqGFIY){UlWXENOjIK = true;}
      if(XkTnMdafar == kWhCfnfPLf){nksELXGnoH = true;}
      else if(kWhCfnfPLf == XkTnMdafar){ILINjfhZIU = true;}
      if(GeeALxMsdF == GoKOLqwbBx){usFZWKZkMN = true;}
      if(FzeKVYoGIb == umqsGbjJhy){RfKeizyAlz = true;}
      if(EsxDpfPXKR == rkrBREPWPJ){ZhWuhuArjd = true;}
      while(GoKOLqwbBx == GeeALxMsdF){DHHQiatJFr = true;}
      while(umqsGbjJhy == umqsGbjJhy){mCkSFXKcdg = true;}
      while(rkrBREPWPJ == rkrBREPWPJ){lUJwuSoxIE = true;}
      if(KBiDJjtpMl == true){KBiDJjtpMl = false;}
      if(xSfbByfouD == true){xSfbByfouD = false;}
      if(EWgNmVwJxH == true){EWgNmVwJxH = false;}
      if(aYLqltDmiV == true){aYLqltDmiV = false;}
      if(gsxwBZEQzS == true){gsxwBZEQzS = false;}
      if(XaaLFmUVQm == true){XaaLFmUVQm = false;}
      if(nksELXGnoH == true){nksELXGnoH = false;}
      if(usFZWKZkMN == true){usFZWKZkMN = false;}
      if(RfKeizyAlz == true){RfKeizyAlz = false;}
      if(ZhWuhuArjd == true){ZhWuhuArjd = false;}
      if(TQaqXeVAot == true){TQaqXeVAot = false;}
      if(NiICVIaNUM == true){NiICVIaNUM = false;}
      if(YFBphMVKra == true){YFBphMVKra = false;}
      if(KgAitAESGE == true){KgAitAESGE = false;}
      if(dxLrkpYTYY == true){dxLrkpYTYY = false;}
      if(UlWXENOjIK == true){UlWXENOjIK = false;}
      if(ILINjfhZIU == true){ILINjfhZIU = false;}
      if(DHHQiatJFr == true){DHHQiatJFr = false;}
      if(mCkSFXKcdg == true){mCkSFXKcdg = false;}
      if(lUJwuSoxIE == true){lUJwuSoxIE = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class GJKBEWXBEE
{ 
  void OYcmjfFdfI()
  { 
      bool fPDBZpfBZB = false;
      bool QlAMVNpETE = false;
      bool MjKNGaoKBn = false;
      bool lbxfONqwtr = false;
      bool KdqIJAxZkc = false;
      bool wUWoywEotz = false;
      bool AVzlOonTxH = false;
      bool WsJbsirCEG = false;
      bool cPYYqREIhR = false;
      bool JCBuqodEtT = false;
      bool LcbcGCKtuO = false;
      bool DCjnVkwauw = false;
      bool kelfyUrOkE = false;
      bool EnIktznuRn = false;
      bool HKNSNGRNYF = false;
      bool rDJxifquel = false;
      bool KuJqKPsysW = false;
      bool xAIGnZRSRq = false;
      bool CeoeLBdKCN = false;
      bool HrQfdLAWIx = false;
      string IUmLZUqPxu;
      string CmUNWtYqfU;
      string YffGHndsVO;
      string mHgQyqGurB;
      string jPUKPBNCPB;
      string WfTlscofEk;
      string pysiIuGDPe;
      string nczdkJKUpF;
      string abdUrGmtXU;
      string CZqGLkVkCU;
      string uJljNsgpBi;
      string RTpzKNUYqB;
      string ILftcNlsRz;
      string nZSVKQosHy;
      string ZwsnDhjruy;
      string QxtBMjdqHz;
      string HmXCurBTnt;
      string XaRraLSqqu;
      string dtxThpeRJa;
      string mxlHpmYqsK;
      if(IUmLZUqPxu == uJljNsgpBi){fPDBZpfBZB = true;}
      else if(uJljNsgpBi == IUmLZUqPxu){LcbcGCKtuO = true;}
      if(CmUNWtYqfU == RTpzKNUYqB){QlAMVNpETE = true;}
      else if(RTpzKNUYqB == CmUNWtYqfU){DCjnVkwauw = true;}
      if(YffGHndsVO == ILftcNlsRz){MjKNGaoKBn = true;}
      else if(ILftcNlsRz == YffGHndsVO){kelfyUrOkE = true;}
      if(mHgQyqGurB == nZSVKQosHy){lbxfONqwtr = true;}
      else if(nZSVKQosHy == mHgQyqGurB){EnIktznuRn = true;}
      if(jPUKPBNCPB == ZwsnDhjruy){KdqIJAxZkc = true;}
      else if(ZwsnDhjruy == jPUKPBNCPB){HKNSNGRNYF = true;}
      if(WfTlscofEk == QxtBMjdqHz){wUWoywEotz = true;}
      else if(QxtBMjdqHz == WfTlscofEk){rDJxifquel = true;}
      if(pysiIuGDPe == HmXCurBTnt){AVzlOonTxH = true;}
      else if(HmXCurBTnt == pysiIuGDPe){KuJqKPsysW = true;}
      if(nczdkJKUpF == XaRraLSqqu){WsJbsirCEG = true;}
      if(abdUrGmtXU == dtxThpeRJa){cPYYqREIhR = true;}
      if(CZqGLkVkCU == mxlHpmYqsK){JCBuqodEtT = true;}
      while(XaRraLSqqu == nczdkJKUpF){xAIGnZRSRq = true;}
      while(dtxThpeRJa == dtxThpeRJa){CeoeLBdKCN = true;}
      while(mxlHpmYqsK == mxlHpmYqsK){HrQfdLAWIx = true;}
      if(fPDBZpfBZB == true){fPDBZpfBZB = false;}
      if(QlAMVNpETE == true){QlAMVNpETE = false;}
      if(MjKNGaoKBn == true){MjKNGaoKBn = false;}
      if(lbxfONqwtr == true){lbxfONqwtr = false;}
      if(KdqIJAxZkc == true){KdqIJAxZkc = false;}
      if(wUWoywEotz == true){wUWoywEotz = false;}
      if(AVzlOonTxH == true){AVzlOonTxH = false;}
      if(WsJbsirCEG == true){WsJbsirCEG = false;}
      if(cPYYqREIhR == true){cPYYqREIhR = false;}
      if(JCBuqodEtT == true){JCBuqodEtT = false;}
      if(LcbcGCKtuO == true){LcbcGCKtuO = false;}
      if(DCjnVkwauw == true){DCjnVkwauw = false;}
      if(kelfyUrOkE == true){kelfyUrOkE = false;}
      if(EnIktznuRn == true){EnIktznuRn = false;}
      if(HKNSNGRNYF == true){HKNSNGRNYF = false;}
      if(rDJxifquel == true){rDJxifquel = false;}
      if(KuJqKPsysW == true){KuJqKPsysW = false;}
      if(xAIGnZRSRq == true){xAIGnZRSRq = false;}
      if(CeoeLBdKCN == true){CeoeLBdKCN = false;}
      if(HrQfdLAWIx == true){HrQfdLAWIx = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SLIUXOGFDX
{ 
  void gxxkBBzByX()
  { 
      bool NaShKkIPru = false;
      bool KmwkRcJAtw = false;
      bool bdFCymKmPB = false;
      bool BVfCjQQifC = false;
      bool RnlTKqIpjH = false;
      bool IbBPSjVPjW = false;
      bool aVApsnHtAA = false;
      bool LaUqdnqkqG = false;
      bool eTCHrZUPiA = false;
      bool uAzIbwnwaW = false;
      bool jNpjwhHJkd = false;
      bool sjYcrGSsxS = false;
      bool SqKzELaLOh = false;
      bool AtKUWQdSLK = false;
      bool SKeFHLbWQV = false;
      bool ctyHDsCpek = false;
      bool GuuDdyZEGa = false;
      bool bAhmAILFrs = false;
      bool iEusEFwCQc = false;
      bool NnmZEXPbbJ = false;
      string SbyIYzVzys;
      string gSINdHtWzN;
      string ehJgPZnJqO;
      string dfyBMmSGwI;
      string gGzpWsLUtj;
      string bebZVjRQsz;
      string LMHjOBUGBp;
      string nnZqfYiSJW;
      string NSPHlzEXfb;
      string VBMxwUzObV;
      string YESEunEksw;
      string AzHVksxAyP;
      string xwaYUlactL;
      string YTqmyIzKLy;
      string tSdibVtqLD;
      string XyQMYbdtiJ;
      string MbqhWfITXc;
      string sloAUdRqxy;
      string qjRpnaDdOV;
      string dFaBkSbuLu;
      if(SbyIYzVzys == YESEunEksw){NaShKkIPru = true;}
      else if(YESEunEksw == SbyIYzVzys){jNpjwhHJkd = true;}
      if(gSINdHtWzN == AzHVksxAyP){KmwkRcJAtw = true;}
      else if(AzHVksxAyP == gSINdHtWzN){sjYcrGSsxS = true;}
      if(ehJgPZnJqO == xwaYUlactL){bdFCymKmPB = true;}
      else if(xwaYUlactL == ehJgPZnJqO){SqKzELaLOh = true;}
      if(dfyBMmSGwI == YTqmyIzKLy){BVfCjQQifC = true;}
      else if(YTqmyIzKLy == dfyBMmSGwI){AtKUWQdSLK = true;}
      if(gGzpWsLUtj == tSdibVtqLD){RnlTKqIpjH = true;}
      else if(tSdibVtqLD == gGzpWsLUtj){SKeFHLbWQV = true;}
      if(bebZVjRQsz == XyQMYbdtiJ){IbBPSjVPjW = true;}
      else if(XyQMYbdtiJ == bebZVjRQsz){ctyHDsCpek = true;}
      if(LMHjOBUGBp == MbqhWfITXc){aVApsnHtAA = true;}
      else if(MbqhWfITXc == LMHjOBUGBp){GuuDdyZEGa = true;}
      if(nnZqfYiSJW == sloAUdRqxy){LaUqdnqkqG = true;}
      if(NSPHlzEXfb == qjRpnaDdOV){eTCHrZUPiA = true;}
      if(VBMxwUzObV == dFaBkSbuLu){uAzIbwnwaW = true;}
      while(sloAUdRqxy == nnZqfYiSJW){bAhmAILFrs = true;}
      while(qjRpnaDdOV == qjRpnaDdOV){iEusEFwCQc = true;}
      while(dFaBkSbuLu == dFaBkSbuLu){NnmZEXPbbJ = true;}
      if(NaShKkIPru == true){NaShKkIPru = false;}
      if(KmwkRcJAtw == true){KmwkRcJAtw = false;}
      if(bdFCymKmPB == true){bdFCymKmPB = false;}
      if(BVfCjQQifC == true){BVfCjQQifC = false;}
      if(RnlTKqIpjH == true){RnlTKqIpjH = false;}
      if(IbBPSjVPjW == true){IbBPSjVPjW = false;}
      if(aVApsnHtAA == true){aVApsnHtAA = false;}
      if(LaUqdnqkqG == true){LaUqdnqkqG = false;}
      if(eTCHrZUPiA == true){eTCHrZUPiA = false;}
      if(uAzIbwnwaW == true){uAzIbwnwaW = false;}
      if(jNpjwhHJkd == true){jNpjwhHJkd = false;}
      if(sjYcrGSsxS == true){sjYcrGSsxS = false;}
      if(SqKzELaLOh == true){SqKzELaLOh = false;}
      if(AtKUWQdSLK == true){AtKUWQdSLK = false;}
      if(SKeFHLbWQV == true){SKeFHLbWQV = false;}
      if(ctyHDsCpek == true){ctyHDsCpek = false;}
      if(GuuDdyZEGa == true){GuuDdyZEGa = false;}
      if(bAhmAILFrs == true){bAhmAILFrs = false;}
      if(iEusEFwCQc == true){iEusEFwCQc = false;}
      if(NnmZEXPbbJ == true){NnmZEXPbbJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class TACZBLXBZA
{ 
  void WOQGNUiHXU()
  { 
      bool WswjLGBOOd = false;
      bool aPqEGYPkCI = false;
      bool eTenxMsdND = false;
      bool OnGAETDxYm = false;
      bool BqLMIIAGwN = false;
      bool nfGwODNHlz = false;
      bool NWAKOmIpzm = false;
      bool USLPHnHSfn = false;
      bool LlgzOigJlB = false;
      bool GuLZYdpAxV = false;
      bool zKGJsVTqoa = false;
      bool utbxnaoAuf = false;
      bool FyDygYHkpc = false;
      bool xHVJnAfXGs = false;
      bool OpULFLTNly = false;
      bool opEQQBGIbT = false;
      bool OhALcnXRIm = false;
      bool uQtCQPdrrV = false;
      bool FszrnHCFsm = false;
      bool sWwmefoIhR = false;
      string zPUmknxtwi;
      string DadBQnpZTb;
      string lwAZVGPlWa;
      string mcRloTdSIV;
      string RMyZAlyYVE;
      string SBUKXDPtQe;
      string XLXVuZJrMx;
      string KWQyKZxaZj;
      string HpPRHlIGOk;
      string EiBcxFncea;
      string bGMYtujWAx;
      string FnUfXhuFNI;
      string RDdaraVmAE;
      string lUuzIBdqXM;
      string BlFQePOUFP;
      string KiPVIXblfT;
      string ZxVwLksVXV;
      string DDhAHexMub;
      string MAgksCFZpj;
      string YqKDmEQJFt;
      if(zPUmknxtwi == bGMYtujWAx){WswjLGBOOd = true;}
      else if(bGMYtujWAx == zPUmknxtwi){zKGJsVTqoa = true;}
      if(DadBQnpZTb == FnUfXhuFNI){aPqEGYPkCI = true;}
      else if(FnUfXhuFNI == DadBQnpZTb){utbxnaoAuf = true;}
      if(lwAZVGPlWa == RDdaraVmAE){eTenxMsdND = true;}
      else if(RDdaraVmAE == lwAZVGPlWa){FyDygYHkpc = true;}
      if(mcRloTdSIV == lUuzIBdqXM){OnGAETDxYm = true;}
      else if(lUuzIBdqXM == mcRloTdSIV){xHVJnAfXGs = true;}
      if(RMyZAlyYVE == BlFQePOUFP){BqLMIIAGwN = true;}
      else if(BlFQePOUFP == RMyZAlyYVE){OpULFLTNly = true;}
      if(SBUKXDPtQe == KiPVIXblfT){nfGwODNHlz = true;}
      else if(KiPVIXblfT == SBUKXDPtQe){opEQQBGIbT = true;}
      if(XLXVuZJrMx == ZxVwLksVXV){NWAKOmIpzm = true;}
      else if(ZxVwLksVXV == XLXVuZJrMx){OhALcnXRIm = true;}
      if(KWQyKZxaZj == DDhAHexMub){USLPHnHSfn = true;}
      if(HpPRHlIGOk == MAgksCFZpj){LlgzOigJlB = true;}
      if(EiBcxFncea == YqKDmEQJFt){GuLZYdpAxV = true;}
      while(DDhAHexMub == KWQyKZxaZj){uQtCQPdrrV = true;}
      while(MAgksCFZpj == MAgksCFZpj){FszrnHCFsm = true;}
      while(YqKDmEQJFt == YqKDmEQJFt){sWwmefoIhR = true;}
      if(WswjLGBOOd == true){WswjLGBOOd = false;}
      if(aPqEGYPkCI == true){aPqEGYPkCI = false;}
      if(eTenxMsdND == true){eTenxMsdND = false;}
      if(OnGAETDxYm == true){OnGAETDxYm = false;}
      if(BqLMIIAGwN == true){BqLMIIAGwN = false;}
      if(nfGwODNHlz == true){nfGwODNHlz = false;}
      if(NWAKOmIpzm == true){NWAKOmIpzm = false;}
      if(USLPHnHSfn == true){USLPHnHSfn = false;}
      if(LlgzOigJlB == true){LlgzOigJlB = false;}
      if(GuLZYdpAxV == true){GuLZYdpAxV = false;}
      if(zKGJsVTqoa == true){zKGJsVTqoa = false;}
      if(utbxnaoAuf == true){utbxnaoAuf = false;}
      if(FyDygYHkpc == true){FyDygYHkpc = false;}
      if(xHVJnAfXGs == true){xHVJnAfXGs = false;}
      if(OpULFLTNly == true){OpULFLTNly = false;}
      if(opEQQBGIbT == true){opEQQBGIbT = false;}
      if(OhALcnXRIm == true){OhALcnXRIm = false;}
      if(uQtCQPdrrV == true){uQtCQPdrrV = false;}
      if(FszrnHCFsm == true){FszrnHCFsm = false;}
      if(sWwmefoIhR == true){sWwmefoIhR = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class TJLCERPFHS
{ 
  void pwiheRMAuc()
  { 
      bool TxstMyWWpw = false;
      bool yGOrmxSXlX = false;
      bool rAMNaFQcJS = false;
      bool GhiqVTAaFq = false;
      bool KBaNyAEnYc = false;
      bool McPaPTjfSH = false;
      bool gsMWpPXWWT = false;
      bool wqSuNqYyOa = false;
      bool AbDgGcSHbC = false;
      bool iUHyJQoEXi = false;
      bool bhRFfUKUNX = false;
      bool tlQqGFpfPN = false;
      bool xPtUjbjUlw = false;
      bool CTafySJxBl = false;
      bool SxsDHxDQqD = false;
      bool aInJkMmxVD = false;
      bool kohQVZKjrm = false;
      bool urdduzVqNm = false;
      bool XTchVseQtl = false;
      bool fMbyBcPcgg = false;
      string dbSAzeJumY;
      string MFIyfEDULs;
      string nxqLgtEVki;
      string AZzQCtMMNR;
      string xbLeqDUiuf;
      string dtnILgjGTE;
      string amKEYlFrul;
      string qpHtuLkMTc;
      string ZUZEEGlqEV;
      string JGsIqKRGGE;
      string xcnJnnmsMz;
      string wXlpeFNFAR;
      string AOmZbkwyDk;
      string teFOfwgPPQ;
      string TrdEZlTDck;
      string wMGnEioOhD;
      string qnpbFQRWKi;
      string XyoiyHkhzo;
      string QsVRogXRKw;
      string HSgMZeBCzI;
      if(dbSAzeJumY == xcnJnnmsMz){TxstMyWWpw = true;}
      else if(xcnJnnmsMz == dbSAzeJumY){bhRFfUKUNX = true;}
      if(MFIyfEDULs == wXlpeFNFAR){yGOrmxSXlX = true;}
      else if(wXlpeFNFAR == MFIyfEDULs){tlQqGFpfPN = true;}
      if(nxqLgtEVki == AOmZbkwyDk){rAMNaFQcJS = true;}
      else if(AOmZbkwyDk == nxqLgtEVki){xPtUjbjUlw = true;}
      if(AZzQCtMMNR == teFOfwgPPQ){GhiqVTAaFq = true;}
      else if(teFOfwgPPQ == AZzQCtMMNR){CTafySJxBl = true;}
      if(xbLeqDUiuf == TrdEZlTDck){KBaNyAEnYc = true;}
      else if(TrdEZlTDck == xbLeqDUiuf){SxsDHxDQqD = true;}
      if(dtnILgjGTE == wMGnEioOhD){McPaPTjfSH = true;}
      else if(wMGnEioOhD == dtnILgjGTE){aInJkMmxVD = true;}
      if(amKEYlFrul == qnpbFQRWKi){gsMWpPXWWT = true;}
      else if(qnpbFQRWKi == amKEYlFrul){kohQVZKjrm = true;}
      if(qpHtuLkMTc == XyoiyHkhzo){wqSuNqYyOa = true;}
      if(ZUZEEGlqEV == QsVRogXRKw){AbDgGcSHbC = true;}
      if(JGsIqKRGGE == HSgMZeBCzI){iUHyJQoEXi = true;}
      while(XyoiyHkhzo == qpHtuLkMTc){urdduzVqNm = true;}
      while(QsVRogXRKw == QsVRogXRKw){XTchVseQtl = true;}
      while(HSgMZeBCzI == HSgMZeBCzI){fMbyBcPcgg = true;}
      if(TxstMyWWpw == true){TxstMyWWpw = false;}
      if(yGOrmxSXlX == true){yGOrmxSXlX = false;}
      if(rAMNaFQcJS == true){rAMNaFQcJS = false;}
      if(GhiqVTAaFq == true){GhiqVTAaFq = false;}
      if(KBaNyAEnYc == true){KBaNyAEnYc = false;}
      if(McPaPTjfSH == true){McPaPTjfSH = false;}
      if(gsMWpPXWWT == true){gsMWpPXWWT = false;}
      if(wqSuNqYyOa == true){wqSuNqYyOa = false;}
      if(AbDgGcSHbC == true){AbDgGcSHbC = false;}
      if(iUHyJQoEXi == true){iUHyJQoEXi = false;}
      if(bhRFfUKUNX == true){bhRFfUKUNX = false;}
      if(tlQqGFpfPN == true){tlQqGFpfPN = false;}
      if(xPtUjbjUlw == true){xPtUjbjUlw = false;}
      if(CTafySJxBl == true){CTafySJxBl = false;}
      if(SxsDHxDQqD == true){SxsDHxDQqD = false;}
      if(aInJkMmxVD == true){aInJkMmxVD = false;}
      if(kohQVZKjrm == true){kohQVZKjrm = false;}
      if(urdduzVqNm == true){urdduzVqNm = false;}
      if(XTchVseQtl == true){XTchVseQtl = false;}
      if(fMbyBcPcgg == true){fMbyBcPcgg = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UICPYOVRGJ
{ 
  void GhutVncAXc()
  { 
      bool JaXLpVHQCZ = false;
      bool MyPtbDRpAQ = false;
      bool OMbVypdlEn = false;
      bool pSpeklUKUw = false;
      bool COhdbSzeDV = false;
      bool uouGzxyBMz = false;
      bool rULNitIkJp = false;
      bool hGVEOAdytf = false;
      bool qziisBWjem = false;
      bool mTxUDgCUtg = false;
      bool KOUNmoSqqz = false;
      bool SAHgIrlcUh = false;
      bool WXGWCOIBAg = false;
      bool yIDMUbdVuD = false;
      bool ifreTlLQLK = false;
      bool tYzMKlIyKY = false;
      bool fRpKBJxuLf = false;
      bool NQhQzqeaCP = false;
      bool CqoomYJqBh = false;
      bool TfKVWQohdJ = false;
      string mYceuuQQun;
      string dYtnUhtoiC;
      string gDrtfybryD;
      string xjUXKZXITx;
      string RglxsJuolS;
      string JItuMjgiyx;
      string TYSrqsDWcu;
      string JkVkkzRtBW;
      string ClGesnpZKa;
      string soKWdrOsGA;
      string FXMqJXmIKM;
      string SlhbtgSnzC;
      string DdLCBTazHr;
      string kYnBQdZOUV;
      string BgOyIHHhbx;
      string ZasDwbMJoo;
      string wnBSasxfKQ;
      string GDScBVONst;
      string MsWKRKekEo;
      string RPZfYMfwWO;
      if(mYceuuQQun == FXMqJXmIKM){JaXLpVHQCZ = true;}
      else if(FXMqJXmIKM == mYceuuQQun){KOUNmoSqqz = true;}
      if(dYtnUhtoiC == SlhbtgSnzC){MyPtbDRpAQ = true;}
      else if(SlhbtgSnzC == dYtnUhtoiC){SAHgIrlcUh = true;}
      if(gDrtfybryD == DdLCBTazHr){OMbVypdlEn = true;}
      else if(DdLCBTazHr == gDrtfybryD){WXGWCOIBAg = true;}
      if(xjUXKZXITx == kYnBQdZOUV){pSpeklUKUw = true;}
      else if(kYnBQdZOUV == xjUXKZXITx){yIDMUbdVuD = true;}
      if(RglxsJuolS == BgOyIHHhbx){COhdbSzeDV = true;}
      else if(BgOyIHHhbx == RglxsJuolS){ifreTlLQLK = true;}
      if(JItuMjgiyx == ZasDwbMJoo){uouGzxyBMz = true;}
      else if(ZasDwbMJoo == JItuMjgiyx){tYzMKlIyKY = true;}
      if(TYSrqsDWcu == wnBSasxfKQ){rULNitIkJp = true;}
      else if(wnBSasxfKQ == TYSrqsDWcu){fRpKBJxuLf = true;}
      if(JkVkkzRtBW == GDScBVONst){hGVEOAdytf = true;}
      if(ClGesnpZKa == MsWKRKekEo){qziisBWjem = true;}
      if(soKWdrOsGA == RPZfYMfwWO){mTxUDgCUtg = true;}
      while(GDScBVONst == JkVkkzRtBW){NQhQzqeaCP = true;}
      while(MsWKRKekEo == MsWKRKekEo){CqoomYJqBh = true;}
      while(RPZfYMfwWO == RPZfYMfwWO){TfKVWQohdJ = true;}
      if(JaXLpVHQCZ == true){JaXLpVHQCZ = false;}
      if(MyPtbDRpAQ == true){MyPtbDRpAQ = false;}
      if(OMbVypdlEn == true){OMbVypdlEn = false;}
      if(pSpeklUKUw == true){pSpeklUKUw = false;}
      if(COhdbSzeDV == true){COhdbSzeDV = false;}
      if(uouGzxyBMz == true){uouGzxyBMz = false;}
      if(rULNitIkJp == true){rULNitIkJp = false;}
      if(hGVEOAdytf == true){hGVEOAdytf = false;}
      if(qziisBWjem == true){qziisBWjem = false;}
      if(mTxUDgCUtg == true){mTxUDgCUtg = false;}
      if(KOUNmoSqqz == true){KOUNmoSqqz = false;}
      if(SAHgIrlcUh == true){SAHgIrlcUh = false;}
      if(WXGWCOIBAg == true){WXGWCOIBAg = false;}
      if(yIDMUbdVuD == true){yIDMUbdVuD = false;}
      if(ifreTlLQLK == true){ifreTlLQLK = false;}
      if(tYzMKlIyKY == true){tYzMKlIyKY = false;}
      if(fRpKBJxuLf == true){fRpKBJxuLf = false;}
      if(NQhQzqeaCP == true){NQhQzqeaCP = false;}
      if(CqoomYJqBh == true){CqoomYJqBh = false;}
      if(TfKVWQohdJ == true){TfKVWQohdJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class RQEBYCOZVF
{ 
  void LcBrQtLFlE()
  { 
      bool OnuVdnkJpx = false;
      bool DtNGSoZgwr = false;
      bool ickLirMsFq = false;
      bool SjzGGhDrVE = false;
      bool xLUtfRXQUS = false;
      bool uIccRkCill = false;
      bool arDzdTuGzk = false;
      bool kgLruXIqfQ = false;
      bool TuXlwKHVoV = false;
      bool eUFjcEHVPM = false;
      bool WRQjgNwjht = false;
      bool cLDVReTRCS = false;
      bool LZRdFQbbOp = false;
      bool PmaStCenEi = false;
      bool OWmtrTQjRy = false;
      bool kpSwAxAmdQ = false;
      bool lUVqdojjaP = false;
      bool AeMTEolWkk = false;
      bool zqJQFZTeRw = false;
      bool CrKnxsAwQD = false;
      string whXnOQKZIy;
      string mnWfSeliRK;
      string WexMMiocKb;
      string GwueRoBMSS;
      string nsKNcdZycE;
      string DhlaFQkKbe;
      string ahPfrprrOc;
      string cZQyVdUijh;
      string IpzFYYKTYA;
      string EqjmqGcdVl;
      string khiAbcQnFb;
      string uBasVjiKHT;
      string fXSNXlpdGP;
      string AqNyxaHzcu;
      string HWOxFCQqQG;
      string XSVCGMOVbo;
      string mmgoaEncwu;
      string cjgMLIJUIV;
      string VyNzrhUKqj;
      string LkWnHUoZVB;
      if(whXnOQKZIy == khiAbcQnFb){OnuVdnkJpx = true;}
      else if(khiAbcQnFb == whXnOQKZIy){WRQjgNwjht = true;}
      if(mnWfSeliRK == uBasVjiKHT){DtNGSoZgwr = true;}
      else if(uBasVjiKHT == mnWfSeliRK){cLDVReTRCS = true;}
      if(WexMMiocKb == fXSNXlpdGP){ickLirMsFq = true;}
      else if(fXSNXlpdGP == WexMMiocKb){LZRdFQbbOp = true;}
      if(GwueRoBMSS == AqNyxaHzcu){SjzGGhDrVE = true;}
      else if(AqNyxaHzcu == GwueRoBMSS){PmaStCenEi = true;}
      if(nsKNcdZycE == HWOxFCQqQG){xLUtfRXQUS = true;}
      else if(HWOxFCQqQG == nsKNcdZycE){OWmtrTQjRy = true;}
      if(DhlaFQkKbe == XSVCGMOVbo){uIccRkCill = true;}
      else if(XSVCGMOVbo == DhlaFQkKbe){kpSwAxAmdQ = true;}
      if(ahPfrprrOc == mmgoaEncwu){arDzdTuGzk = true;}
      else if(mmgoaEncwu == ahPfrprrOc){lUVqdojjaP = true;}
      if(cZQyVdUijh == cjgMLIJUIV){kgLruXIqfQ = true;}
      if(IpzFYYKTYA == VyNzrhUKqj){TuXlwKHVoV = true;}
      if(EqjmqGcdVl == LkWnHUoZVB){eUFjcEHVPM = true;}
      while(cjgMLIJUIV == cZQyVdUijh){AeMTEolWkk = true;}
      while(VyNzrhUKqj == VyNzrhUKqj){zqJQFZTeRw = true;}
      while(LkWnHUoZVB == LkWnHUoZVB){CrKnxsAwQD = true;}
      if(OnuVdnkJpx == true){OnuVdnkJpx = false;}
      if(DtNGSoZgwr == true){DtNGSoZgwr = false;}
      if(ickLirMsFq == true){ickLirMsFq = false;}
      if(SjzGGhDrVE == true){SjzGGhDrVE = false;}
      if(xLUtfRXQUS == true){xLUtfRXQUS = false;}
      if(uIccRkCill == true){uIccRkCill = false;}
      if(arDzdTuGzk == true){arDzdTuGzk = false;}
      if(kgLruXIqfQ == true){kgLruXIqfQ = false;}
      if(TuXlwKHVoV == true){TuXlwKHVoV = false;}
      if(eUFjcEHVPM == true){eUFjcEHVPM = false;}
      if(WRQjgNwjht == true){WRQjgNwjht = false;}
      if(cLDVReTRCS == true){cLDVReTRCS = false;}
      if(LZRdFQbbOp == true){LZRdFQbbOp = false;}
      if(PmaStCenEi == true){PmaStCenEi = false;}
      if(OWmtrTQjRy == true){OWmtrTQjRy = false;}
      if(kpSwAxAmdQ == true){kpSwAxAmdQ = false;}
      if(lUVqdojjaP == true){lUVqdojjaP = false;}
      if(AeMTEolWkk == true){AeMTEolWkk = false;}
      if(zqJQFZTeRw == true){zqJQFZTeRw = false;}
      if(CrKnxsAwQD == true){CrKnxsAwQD = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZJYFFZVHSH
{ 
  void hQizJzGLlg()
  { 
      bool SllEbZrjnI = false;
      bool FqVmGrVrWw = false;
      bool hefZqficnT = false;
      bool XFoaMQoPcz = false;
      bool tEkuDaqkHY = false;
      bool GIspYnFLMu = false;
      bool qOuUkABuao = false;
      bool DFOWBdskWP = false;
      bool nyVPNOLzCX = false;
      bool KcCgIfIPJD = false;
      bool ZwKRUAtWou = false;
      bool tUoxHrsfHW = false;
      bool oBsCdZDgNU = false;
      bool UcbTornDtM = false;
      bool ToKGdpnFZQ = false;
      bool mcImNYDHmu = false;
      bool xNITZQrVHP = false;
      bool EoktCAqlYm = false;
      bool kxGxhrUoFQ = false;
      bool nfQzPGfpgM = false;
      string FoFNmodryN;
      string EFaozboyqZ;
      string woPrApsUQE;
      string HJGCxKHCGf;
      string mabFAHnJJG;
      string CQkpbQYNNx;
      string wIasGuyXro;
      string UlrLlVNKqs;
      string BiLKBqXQaf;
      string lsoJiezhzF;
      string IQQUlQYZDF;
      string COWTuMxPqO;
      string WhxLCawhbH;
      string VfKljkSpYk;
      string SyMUdWWVOV;
      string gipkhaSESK;
      string TpxwiWEjiJ;
      string tHqGwYUbhe;
      string DaTFZmoZDJ;
      string SNXNRbBPlG;
      if(FoFNmodryN == IQQUlQYZDF){SllEbZrjnI = true;}
      else if(IQQUlQYZDF == FoFNmodryN){ZwKRUAtWou = true;}
      if(EFaozboyqZ == COWTuMxPqO){FqVmGrVrWw = true;}
      else if(COWTuMxPqO == EFaozboyqZ){tUoxHrsfHW = true;}
      if(woPrApsUQE == WhxLCawhbH){hefZqficnT = true;}
      else if(WhxLCawhbH == woPrApsUQE){oBsCdZDgNU = true;}
      if(HJGCxKHCGf == VfKljkSpYk){XFoaMQoPcz = true;}
      else if(VfKljkSpYk == HJGCxKHCGf){UcbTornDtM = true;}
      if(mabFAHnJJG == SyMUdWWVOV){tEkuDaqkHY = true;}
      else if(SyMUdWWVOV == mabFAHnJJG){ToKGdpnFZQ = true;}
      if(CQkpbQYNNx == gipkhaSESK){GIspYnFLMu = true;}
      else if(gipkhaSESK == CQkpbQYNNx){mcImNYDHmu = true;}
      if(wIasGuyXro == TpxwiWEjiJ){qOuUkABuao = true;}
      else if(TpxwiWEjiJ == wIasGuyXro){xNITZQrVHP = true;}
      if(UlrLlVNKqs == tHqGwYUbhe){DFOWBdskWP = true;}
      if(BiLKBqXQaf == DaTFZmoZDJ){nyVPNOLzCX = true;}
      if(lsoJiezhzF == SNXNRbBPlG){KcCgIfIPJD = true;}
      while(tHqGwYUbhe == UlrLlVNKqs){EoktCAqlYm = true;}
      while(DaTFZmoZDJ == DaTFZmoZDJ){kxGxhrUoFQ = true;}
      while(SNXNRbBPlG == SNXNRbBPlG){nfQzPGfpgM = true;}
      if(SllEbZrjnI == true){SllEbZrjnI = false;}
      if(FqVmGrVrWw == true){FqVmGrVrWw = false;}
      if(hefZqficnT == true){hefZqficnT = false;}
      if(XFoaMQoPcz == true){XFoaMQoPcz = false;}
      if(tEkuDaqkHY == true){tEkuDaqkHY = false;}
      if(GIspYnFLMu == true){GIspYnFLMu = false;}
      if(qOuUkABuao == true){qOuUkABuao = false;}
      if(DFOWBdskWP == true){DFOWBdskWP = false;}
      if(nyVPNOLzCX == true){nyVPNOLzCX = false;}
      if(KcCgIfIPJD == true){KcCgIfIPJD = false;}
      if(ZwKRUAtWou == true){ZwKRUAtWou = false;}
      if(tUoxHrsfHW == true){tUoxHrsfHW = false;}
      if(oBsCdZDgNU == true){oBsCdZDgNU = false;}
      if(UcbTornDtM == true){UcbTornDtM = false;}
      if(ToKGdpnFZQ == true){ToKGdpnFZQ = false;}
      if(mcImNYDHmu == true){mcImNYDHmu = false;}
      if(xNITZQrVHP == true){xNITZQrVHP = false;}
      if(EoktCAqlYm == true){EoktCAqlYm = false;}
      if(kxGxhrUoFQ == true){kxGxhrUoFQ = false;}
      if(nfQzPGfpgM == true){nfQzPGfpgM = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class YFFBDDDKQW
{ 
  void ARzhYyEsRb()
  { 
      bool ucuWJKzgjZ = false;
      bool kPuFHipOYF = false;
      bool plmTtlLTBn = false;
      bool TyGHRKeZyp = false;
      bool HPpcUTfCqG = false;
      bool DmLclJpbbX = false;
      bool yeewFmInmw = false;
      bool DPPixauKya = false;
      bool FJctkfmdVI = false;
      bool UCxaqcjtWh = false;
      bool KrOtCmXbeK = false;
      bool NedBRoBjLY = false;
      bool zdSeaBSnrU = false;
      bool qBmPPMthrx = false;
      bool IKzKtpYwIB = false;
      bool azhyhfZiTB = false;
      bool UijKTLrNkF = false;
      bool uSrNQneHjk = false;
      bool bWepsrSogj = false;
      bool mKOpcYanqi = false;
      string YLaHeECaVp;
      string WgkeeAkagn;
      string olfyOjrqRc;
      string NBXTIsjQLG;
      string jmFHKXGCaT;
      string ZxbImmecrt;
      string MLJscUJQVk;
      string PTOZDBfYnp;
      string coQxDPZPgV;
      string OHXRtYcMep;
      string OofoDFiBEk;
      string GGnCQWtLoM;
      string ttjAVHwwee;
      string YTsybnIasy;
      string fXKuPITVAX;
      string dRXRuUEfSu;
      string IlVZMmMpZc;
      string NTRpgViojN;
      string nZwHAHwaCG;
      string ibHyosCjhc;
      if(YLaHeECaVp == OofoDFiBEk){ucuWJKzgjZ = true;}
      else if(OofoDFiBEk == YLaHeECaVp){KrOtCmXbeK = true;}
      if(WgkeeAkagn == GGnCQWtLoM){kPuFHipOYF = true;}
      else if(GGnCQWtLoM == WgkeeAkagn){NedBRoBjLY = true;}
      if(olfyOjrqRc == ttjAVHwwee){plmTtlLTBn = true;}
      else if(ttjAVHwwee == olfyOjrqRc){zdSeaBSnrU = true;}
      if(NBXTIsjQLG == YTsybnIasy){TyGHRKeZyp = true;}
      else if(YTsybnIasy == NBXTIsjQLG){qBmPPMthrx = true;}
      if(jmFHKXGCaT == fXKuPITVAX){HPpcUTfCqG = true;}
      else if(fXKuPITVAX == jmFHKXGCaT){IKzKtpYwIB = true;}
      if(ZxbImmecrt == dRXRuUEfSu){DmLclJpbbX = true;}
      else if(dRXRuUEfSu == ZxbImmecrt){azhyhfZiTB = true;}
      if(MLJscUJQVk == IlVZMmMpZc){yeewFmInmw = true;}
      else if(IlVZMmMpZc == MLJscUJQVk){UijKTLrNkF = true;}
      if(PTOZDBfYnp == NTRpgViojN){DPPixauKya = true;}
      if(coQxDPZPgV == nZwHAHwaCG){FJctkfmdVI = true;}
      if(OHXRtYcMep == ibHyosCjhc){UCxaqcjtWh = true;}
      while(NTRpgViojN == PTOZDBfYnp){uSrNQneHjk = true;}
      while(nZwHAHwaCG == nZwHAHwaCG){bWepsrSogj = true;}
      while(ibHyosCjhc == ibHyosCjhc){mKOpcYanqi = true;}
      if(ucuWJKzgjZ == true){ucuWJKzgjZ = false;}
      if(kPuFHipOYF == true){kPuFHipOYF = false;}
      if(plmTtlLTBn == true){plmTtlLTBn = false;}
      if(TyGHRKeZyp == true){TyGHRKeZyp = false;}
      if(HPpcUTfCqG == true){HPpcUTfCqG = false;}
      if(DmLclJpbbX == true){DmLclJpbbX = false;}
      if(yeewFmInmw == true){yeewFmInmw = false;}
      if(DPPixauKya == true){DPPixauKya = false;}
      if(FJctkfmdVI == true){FJctkfmdVI = false;}
      if(UCxaqcjtWh == true){UCxaqcjtWh = false;}
      if(KrOtCmXbeK == true){KrOtCmXbeK = false;}
      if(NedBRoBjLY == true){NedBRoBjLY = false;}
      if(zdSeaBSnrU == true){zdSeaBSnrU = false;}
      if(qBmPPMthrx == true){qBmPPMthrx = false;}
      if(IKzKtpYwIB == true){IKzKtpYwIB = false;}
      if(azhyhfZiTB == true){azhyhfZiTB = false;}
      if(UijKTLrNkF == true){UijKTLrNkF = false;}
      if(uSrNQneHjk == true){uSrNQneHjk = false;}
      if(bWepsrSogj == true){bWepsrSogj = false;}
      if(mKOpcYanqi == true){mKOpcYanqi = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CIDRDRJBVT
{ 
  void FmcmFLBcHq()
  { 
      bool bZLgDyUiOB = false;
      bool qVRfGiHdnz = false;
      bool hSLILOnuLH = false;
      bool ArpKVKYQLH = false;
      bool JfhmfdoyVx = false;
      bool fDBYdJNCUo = false;
      bool KZWHByeHJu = false;
      bool KpRfpBjYbh = false;
      bool ZoZNbxqRgL = false;
      bool xVwWwjIalg = false;
      bool yhnZmokUib = false;
      bool ZoZlPUzIfD = false;
      bool HNZyxntujF = false;
      bool rIlujqBzal = false;
      bool MVhEkIuJHa = false;
      bool jAuiYkIocy = false;
      bool uokcjRCeCX = false;
      bool TAhsqleUMo = false;
      bool rjnIVrMDVz = false;
      bool iLWcqWjEUT = false;
      string QMZUOmIwew;
      string ACoQAHjKII;
      string yTigCYGFko;
      string RegOqttmeY;
      string tsJcOMgqbP;
      string rbTeHwdsSo;
      string gDKodUUaYG;
      string MwfiNFJGSo;
      string rOcUwoLBGt;
      string PBUorqhTys;
      string rGSUCuwjRC;
      string lXacxRZpsn;
      string JSTdpLbppW;
      string WpHYxkuISA;
      string sajEHraqrZ;
      string QraJjZlndq;
      string TFttewPBfL;
      string VRWruhhPVy;
      string jjyPGeOxoX;
      string zHdbgmgoYL;
      if(QMZUOmIwew == rGSUCuwjRC){bZLgDyUiOB = true;}
      else if(rGSUCuwjRC == QMZUOmIwew){yhnZmokUib = true;}
      if(ACoQAHjKII == lXacxRZpsn){qVRfGiHdnz = true;}
      else if(lXacxRZpsn == ACoQAHjKII){ZoZlPUzIfD = true;}
      if(yTigCYGFko == JSTdpLbppW){hSLILOnuLH = true;}
      else if(JSTdpLbppW == yTigCYGFko){HNZyxntujF = true;}
      if(RegOqttmeY == WpHYxkuISA){ArpKVKYQLH = true;}
      else if(WpHYxkuISA == RegOqttmeY){rIlujqBzal = true;}
      if(tsJcOMgqbP == sajEHraqrZ){JfhmfdoyVx = true;}
      else if(sajEHraqrZ == tsJcOMgqbP){MVhEkIuJHa = true;}
      if(rbTeHwdsSo == QraJjZlndq){fDBYdJNCUo = true;}
      else if(QraJjZlndq == rbTeHwdsSo){jAuiYkIocy = true;}
      if(gDKodUUaYG == TFttewPBfL){KZWHByeHJu = true;}
      else if(TFttewPBfL == gDKodUUaYG){uokcjRCeCX = true;}
      if(MwfiNFJGSo == VRWruhhPVy){KpRfpBjYbh = true;}
      if(rOcUwoLBGt == jjyPGeOxoX){ZoZNbxqRgL = true;}
      if(PBUorqhTys == zHdbgmgoYL){xVwWwjIalg = true;}
      while(VRWruhhPVy == MwfiNFJGSo){TAhsqleUMo = true;}
      while(jjyPGeOxoX == jjyPGeOxoX){rjnIVrMDVz = true;}
      while(zHdbgmgoYL == zHdbgmgoYL){iLWcqWjEUT = true;}
      if(bZLgDyUiOB == true){bZLgDyUiOB = false;}
      if(qVRfGiHdnz == true){qVRfGiHdnz = false;}
      if(hSLILOnuLH == true){hSLILOnuLH = false;}
      if(ArpKVKYQLH == true){ArpKVKYQLH = false;}
      if(JfhmfdoyVx == true){JfhmfdoyVx = false;}
      if(fDBYdJNCUo == true){fDBYdJNCUo = false;}
      if(KZWHByeHJu == true){KZWHByeHJu = false;}
      if(KpRfpBjYbh == true){KpRfpBjYbh = false;}
      if(ZoZNbxqRgL == true){ZoZNbxqRgL = false;}
      if(xVwWwjIalg == true){xVwWwjIalg = false;}
      if(yhnZmokUib == true){yhnZmokUib = false;}
      if(ZoZlPUzIfD == true){ZoZlPUzIfD = false;}
      if(HNZyxntujF == true){HNZyxntujF = false;}
      if(rIlujqBzal == true){rIlujqBzal = false;}
      if(MVhEkIuJHa == true){MVhEkIuJHa = false;}
      if(jAuiYkIocy == true){jAuiYkIocy = false;}
      if(uokcjRCeCX == true){uokcjRCeCX = false;}
      if(TAhsqleUMo == true){TAhsqleUMo = false;}
      if(rjnIVrMDVz == true){rjnIVrMDVz = false;}
      if(iLWcqWjEUT == true){iLWcqWjEUT = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NKOGISAHKM
{ 
  void JKdUytnrhy()
  { 
      bool bRcbmFbJhu = false;
      bool UorkCuPuMs = false;
      bool uwhDHIcPLR = false;
      bool KIixaxWUxu = false;
      bool KddZAdqjWx = false;
      bool PsVInfjYTe = false;
      bool bMyQkLGtJE = false;
      bool YtyQiQRHCd = false;
      bool RrrNBQmZIu = false;
      bool NxBRTyfNon = false;
      bool WwZSYVuEZX = false;
      bool NcVpNjTjOU = false;
      bool YgkOqasfxj = false;
      bool jDSOHURZof = false;
      bool UhAkgkbKEi = false;
      bool dwzTOpKLbx = false;
      bool bPGomJJHir = false;
      bool RtuizqfzgG = false;
      bool zhaOGnEyzQ = false;
      bool JGyqDFbLyX = false;
      string hkeaYquEOk;
      string wNnxVPCCMk;
      string aoZfnRItbm;
      string pLHCuunteP;
      string PZhOAGeaEf;
      string DjtpkrKzmH;
      string hAcbukGkCZ;
      string aHWxVAHKeg;
      string mgpwiFQRRC;
      string koHWgPxGTO;
      string AsYdEuMwUj;
      string jzTFUAKfro;
      string TnSzolAAQF;
      string DBqQgsILqn;
      string rUxhSkVBCf;
      string EQNeuLZYoS;
      string tMfOfiIDYa;
      string rznExzqkgD;
      string PzxkBmmLVU;
      string SiVYsfoiCi;
      if(hkeaYquEOk == AsYdEuMwUj){bRcbmFbJhu = true;}
      else if(AsYdEuMwUj == hkeaYquEOk){WwZSYVuEZX = true;}
      if(wNnxVPCCMk == jzTFUAKfro){UorkCuPuMs = true;}
      else if(jzTFUAKfro == wNnxVPCCMk){NcVpNjTjOU = true;}
      if(aoZfnRItbm == TnSzolAAQF){uwhDHIcPLR = true;}
      else if(TnSzolAAQF == aoZfnRItbm){YgkOqasfxj = true;}
      if(pLHCuunteP == DBqQgsILqn){KIixaxWUxu = true;}
      else if(DBqQgsILqn == pLHCuunteP){jDSOHURZof = true;}
      if(PZhOAGeaEf == rUxhSkVBCf){KddZAdqjWx = true;}
      else if(rUxhSkVBCf == PZhOAGeaEf){UhAkgkbKEi = true;}
      if(DjtpkrKzmH == EQNeuLZYoS){PsVInfjYTe = true;}
      else if(EQNeuLZYoS == DjtpkrKzmH){dwzTOpKLbx = true;}
      if(hAcbukGkCZ == tMfOfiIDYa){bMyQkLGtJE = true;}
      else if(tMfOfiIDYa == hAcbukGkCZ){bPGomJJHir = true;}
      if(aHWxVAHKeg == rznExzqkgD){YtyQiQRHCd = true;}
      if(mgpwiFQRRC == PzxkBmmLVU){RrrNBQmZIu = true;}
      if(koHWgPxGTO == SiVYsfoiCi){NxBRTyfNon = true;}
      while(rznExzqkgD == aHWxVAHKeg){RtuizqfzgG = true;}
      while(PzxkBmmLVU == PzxkBmmLVU){zhaOGnEyzQ = true;}
      while(SiVYsfoiCi == SiVYsfoiCi){JGyqDFbLyX = true;}
      if(bRcbmFbJhu == true){bRcbmFbJhu = false;}
      if(UorkCuPuMs == true){UorkCuPuMs = false;}
      if(uwhDHIcPLR == true){uwhDHIcPLR = false;}
      if(KIixaxWUxu == true){KIixaxWUxu = false;}
      if(KddZAdqjWx == true){KddZAdqjWx = false;}
      if(PsVInfjYTe == true){PsVInfjYTe = false;}
      if(bMyQkLGtJE == true){bMyQkLGtJE = false;}
      if(YtyQiQRHCd == true){YtyQiQRHCd = false;}
      if(RrrNBQmZIu == true){RrrNBQmZIu = false;}
      if(NxBRTyfNon == true){NxBRTyfNon = false;}
      if(WwZSYVuEZX == true){WwZSYVuEZX = false;}
      if(NcVpNjTjOU == true){NcVpNjTjOU = false;}
      if(YgkOqasfxj == true){YgkOqasfxj = false;}
      if(jDSOHURZof == true){jDSOHURZof = false;}
      if(UhAkgkbKEi == true){UhAkgkbKEi = false;}
      if(dwzTOpKLbx == true){dwzTOpKLbx = false;}
      if(bPGomJJHir == true){bPGomJJHir = false;}
      if(RtuizqfzgG == true){RtuizqfzgG = false;}
      if(zhaOGnEyzQ == true){zhaOGnEyzQ = false;}
      if(JGyqDFbLyX == true){JGyqDFbLyX = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JXFXTNDNLV
{ 
  void IEESssZOya()
  { 
      bool TiJoMlMpdm = false;
      bool HtOfONTHsg = false;
      bool JpmeWdALGp = false;
      bool mogipcEHgS = false;
      bool KtuVtTOcNp = false;
      bool VXLjJdwdRh = false;
      bool lAinzRNepl = false;
      bool JLZaVPtRjD = false;
      bool PWxPTPcwZL = false;
      bool gZoyfKKGWb = false;
      bool GimbgJxkNs = false;
      bool jdtOSOCTUi = false;
      bool nQFeFiVbtL = false;
      bool EntHxfjViJ = false;
      bool SrifbCfafR = false;
      bool HqlmmodHVx = false;
      bool nVqBnujlQl = false;
      bool efjKPwgYDD = false;
      bool DtubDNCGTH = false;
      bool CbzlVxAgqf = false;
      string ejoGHLtpjE;
      string kYRJmEDPyP;
      string fXGLNyaxZe;
      string XtSyzLUUaK;
      string ELwbtHftMC;
      string PcBMyGccrI;
      string kwQFfpFTKb;
      string ihomKTbeRG;
      string RfkWOtsGzs;
      string hqcRZIuquD;
      string BMjjShnNBd;
      string eyLlqEKsql;
      string xzTRtrFXNC;
      string YjiOEFwAEf;
      string GeEVNifqZU;
      string xBAJFCrSpa;
      string bQAzEtOwgk;
      string IKoKeSuyDK;
      string LOxsFWjYIC;
      string NdJcEoeFQT;
      if(ejoGHLtpjE == BMjjShnNBd){TiJoMlMpdm = true;}
      else if(BMjjShnNBd == ejoGHLtpjE){GimbgJxkNs = true;}
      if(kYRJmEDPyP == eyLlqEKsql){HtOfONTHsg = true;}
      else if(eyLlqEKsql == kYRJmEDPyP){jdtOSOCTUi = true;}
      if(fXGLNyaxZe == xzTRtrFXNC){JpmeWdALGp = true;}
      else if(xzTRtrFXNC == fXGLNyaxZe){nQFeFiVbtL = true;}
      if(XtSyzLUUaK == YjiOEFwAEf){mogipcEHgS = true;}
      else if(YjiOEFwAEf == XtSyzLUUaK){EntHxfjViJ = true;}
      if(ELwbtHftMC == GeEVNifqZU){KtuVtTOcNp = true;}
      else if(GeEVNifqZU == ELwbtHftMC){SrifbCfafR = true;}
      if(PcBMyGccrI == xBAJFCrSpa){VXLjJdwdRh = true;}
      else if(xBAJFCrSpa == PcBMyGccrI){HqlmmodHVx = true;}
      if(kwQFfpFTKb == bQAzEtOwgk){lAinzRNepl = true;}
      else if(bQAzEtOwgk == kwQFfpFTKb){nVqBnujlQl = true;}
      if(ihomKTbeRG == IKoKeSuyDK){JLZaVPtRjD = true;}
      if(RfkWOtsGzs == LOxsFWjYIC){PWxPTPcwZL = true;}
      if(hqcRZIuquD == NdJcEoeFQT){gZoyfKKGWb = true;}
      while(IKoKeSuyDK == ihomKTbeRG){efjKPwgYDD = true;}
      while(LOxsFWjYIC == LOxsFWjYIC){DtubDNCGTH = true;}
      while(NdJcEoeFQT == NdJcEoeFQT){CbzlVxAgqf = true;}
      if(TiJoMlMpdm == true){TiJoMlMpdm = false;}
      if(HtOfONTHsg == true){HtOfONTHsg = false;}
      if(JpmeWdALGp == true){JpmeWdALGp = false;}
      if(mogipcEHgS == true){mogipcEHgS = false;}
      if(KtuVtTOcNp == true){KtuVtTOcNp = false;}
      if(VXLjJdwdRh == true){VXLjJdwdRh = false;}
      if(lAinzRNepl == true){lAinzRNepl = false;}
      if(JLZaVPtRjD == true){JLZaVPtRjD = false;}
      if(PWxPTPcwZL == true){PWxPTPcwZL = false;}
      if(gZoyfKKGWb == true){gZoyfKKGWb = false;}
      if(GimbgJxkNs == true){GimbgJxkNs = false;}
      if(jdtOSOCTUi == true){jdtOSOCTUi = false;}
      if(nQFeFiVbtL == true){nQFeFiVbtL = false;}
      if(EntHxfjViJ == true){EntHxfjViJ = false;}
      if(SrifbCfafR == true){SrifbCfafR = false;}
      if(HqlmmodHVx == true){HqlmmodHVx = false;}
      if(nVqBnujlQl == true){nVqBnujlQl = false;}
      if(efjKPwgYDD == true){efjKPwgYDD = false;}
      if(DtubDNCGTH == true){DtubDNCGTH = false;}
      if(CbzlVxAgqf == true){CbzlVxAgqf = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PTFZPMLTZG
{ 
  void xqDaxMQdaB()
  { 
      bool OYrTbApggh = false;
      bool NkbaCXiQOc = false;
      bool xTDGgtOwPt = false;
      bool plQnYBXlSA = false;
      bool oPGPStjBTi = false;
      bool IcASstVlmh = false;
      bool ksRmqbUnBA = false;
      bool zxxOmhNmmm = false;
      bool JMcfkNlxtd = false;
      bool flyjPuxSYq = false;
      bool YwASiLeleL = false;
      bool ghyAYYQlCd = false;
      bool oWsWUNyMzr = false;
      bool kuOwNMMJwY = false;
      bool hASixxnyLp = false;
      bool eLQtBBCcyV = false;
      bool NRmmalSBmL = false;
      bool MGocwoEKZW = false;
      bool YABIMYdUtL = false;
      bool KiLZLPMeIL = false;
      string cluuBuwyih;
      string CtVLDYFOTA;
      string iEWIQTYtCf;
      string zUVHIDetEf;
      string eVcXmWbcaY;
      string xnbfIYPmdg;
      string WCuDhgYOCz;
      string tLKmVNbyUt;
      string quSdiLspyj;
      string nrPWJbZZpF;
      string xNaaekcnKu;
      string xQRjpajRUV;
      string nhPSLBwmcs;
      string mCIPwdUrzx;
      string iEfDmWafgh;
      string rIEURPxHOc;
      string opLQHuIBmM;
      string GqSXmiBLfn;
      string aQGuBexnyg;
      string fMcVHrsPuq;
      if(cluuBuwyih == xNaaekcnKu){OYrTbApggh = true;}
      else if(xNaaekcnKu == cluuBuwyih){YwASiLeleL = true;}
      if(CtVLDYFOTA == xQRjpajRUV){NkbaCXiQOc = true;}
      else if(xQRjpajRUV == CtVLDYFOTA){ghyAYYQlCd = true;}
      if(iEWIQTYtCf == nhPSLBwmcs){xTDGgtOwPt = true;}
      else if(nhPSLBwmcs == iEWIQTYtCf){oWsWUNyMzr = true;}
      if(zUVHIDetEf == mCIPwdUrzx){plQnYBXlSA = true;}
      else if(mCIPwdUrzx == zUVHIDetEf){kuOwNMMJwY = true;}
      if(eVcXmWbcaY == iEfDmWafgh){oPGPStjBTi = true;}
      else if(iEfDmWafgh == eVcXmWbcaY){hASixxnyLp = true;}
      if(xnbfIYPmdg == rIEURPxHOc){IcASstVlmh = true;}
      else if(rIEURPxHOc == xnbfIYPmdg){eLQtBBCcyV = true;}
      if(WCuDhgYOCz == opLQHuIBmM){ksRmqbUnBA = true;}
      else if(opLQHuIBmM == WCuDhgYOCz){NRmmalSBmL = true;}
      if(tLKmVNbyUt == GqSXmiBLfn){zxxOmhNmmm = true;}
      if(quSdiLspyj == aQGuBexnyg){JMcfkNlxtd = true;}
      if(nrPWJbZZpF == fMcVHrsPuq){flyjPuxSYq = true;}
      while(GqSXmiBLfn == tLKmVNbyUt){MGocwoEKZW = true;}
      while(aQGuBexnyg == aQGuBexnyg){YABIMYdUtL = true;}
      while(fMcVHrsPuq == fMcVHrsPuq){KiLZLPMeIL = true;}
      if(OYrTbApggh == true){OYrTbApggh = false;}
      if(NkbaCXiQOc == true){NkbaCXiQOc = false;}
      if(xTDGgtOwPt == true){xTDGgtOwPt = false;}
      if(plQnYBXlSA == true){plQnYBXlSA = false;}
      if(oPGPStjBTi == true){oPGPStjBTi = false;}
      if(IcASstVlmh == true){IcASstVlmh = false;}
      if(ksRmqbUnBA == true){ksRmqbUnBA = false;}
      if(zxxOmhNmmm == true){zxxOmhNmmm = false;}
      if(JMcfkNlxtd == true){JMcfkNlxtd = false;}
      if(flyjPuxSYq == true){flyjPuxSYq = false;}
      if(YwASiLeleL == true){YwASiLeleL = false;}
      if(ghyAYYQlCd == true){ghyAYYQlCd = false;}
      if(oWsWUNyMzr == true){oWsWUNyMzr = false;}
      if(kuOwNMMJwY == true){kuOwNMMJwY = false;}
      if(hASixxnyLp == true){hASixxnyLp = false;}
      if(eLQtBBCcyV == true){eLQtBBCcyV = false;}
      if(NRmmalSBmL == true){NRmmalSBmL = false;}
      if(MGocwoEKZW == true){MGocwoEKZW = false;}
      if(YABIMYdUtL == true){YABIMYdUtL = false;}
      if(KiLZLPMeIL == true){KiLZLPMeIL = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PELPASIIXY
{ 
  void zKzQQuFgCG()
  { 
      bool rxKrXqTzaN = false;
      bool EOFeXJEUIc = false;
      bool VpUBizqFfG = false;
      bool QrsChKXGrx = false;
      bool WPYamUkesd = false;
      bool rqMyHBcJIY = false;
      bool zOTTxLVCwN = false;
      bool kybLGGRsNN = false;
      bool NMLBOZfttg = false;
      bool gNxTxIuEda = false;
      bool zhhfssOArB = false;
      bool jiutKmCSEX = false;
      bool eaIPfWAIOx = false;
      bool LMPlOfdfHV = false;
      bool LfCswxRDIK = false;
      bool GcAGwXZSzV = false;
      bool sFktoWTJfe = false;
      bool IVKiLzXOSi = false;
      bool FxzEqiMJpp = false;
      bool GLglbFtIDY = false;
      string FpbCeMzKsC;
      string bwmpppKPdu;
      string GhKsoYeDAh;
      string kqReHfdSFP;
      string FSezAeGRYH;
      string WXdUlWehMG;
      string guKLQkeIhJ;
      string LrJNpmCLsK;
      string SNerKNMxsj;
      string BwFziJHThO;
      string AyjOueNPdy;
      string USnCmTQSAD;
      string YgOxIPlKeg;
      string eideJTdPwf;
      string zwStPhHDLO;
      string roTdpSilFf;
      string rDkxSkoJrT;
      string pBVTDbOUga;
      string IiyQnpOzoq;
      string jqJQnwLVad;
      if(FpbCeMzKsC == AyjOueNPdy){rxKrXqTzaN = true;}
      else if(AyjOueNPdy == FpbCeMzKsC){zhhfssOArB = true;}
      if(bwmpppKPdu == USnCmTQSAD){EOFeXJEUIc = true;}
      else if(USnCmTQSAD == bwmpppKPdu){jiutKmCSEX = true;}
      if(GhKsoYeDAh == YgOxIPlKeg){VpUBizqFfG = true;}
      else if(YgOxIPlKeg == GhKsoYeDAh){eaIPfWAIOx = true;}
      if(kqReHfdSFP == eideJTdPwf){QrsChKXGrx = true;}
      else if(eideJTdPwf == kqReHfdSFP){LMPlOfdfHV = true;}
      if(FSezAeGRYH == zwStPhHDLO){WPYamUkesd = true;}
      else if(zwStPhHDLO == FSezAeGRYH){LfCswxRDIK = true;}
      if(WXdUlWehMG == roTdpSilFf){rqMyHBcJIY = true;}
      else if(roTdpSilFf == WXdUlWehMG){GcAGwXZSzV = true;}
      if(guKLQkeIhJ == rDkxSkoJrT){zOTTxLVCwN = true;}
      else if(rDkxSkoJrT == guKLQkeIhJ){sFktoWTJfe = true;}
      if(LrJNpmCLsK == pBVTDbOUga){kybLGGRsNN = true;}
      if(SNerKNMxsj == IiyQnpOzoq){NMLBOZfttg = true;}
      if(BwFziJHThO == jqJQnwLVad){gNxTxIuEda = true;}
      while(pBVTDbOUga == LrJNpmCLsK){IVKiLzXOSi = true;}
      while(IiyQnpOzoq == IiyQnpOzoq){FxzEqiMJpp = true;}
      while(jqJQnwLVad == jqJQnwLVad){GLglbFtIDY = true;}
      if(rxKrXqTzaN == true){rxKrXqTzaN = false;}
      if(EOFeXJEUIc == true){EOFeXJEUIc = false;}
      if(VpUBizqFfG == true){VpUBizqFfG = false;}
      if(QrsChKXGrx == true){QrsChKXGrx = false;}
      if(WPYamUkesd == true){WPYamUkesd = false;}
      if(rqMyHBcJIY == true){rqMyHBcJIY = false;}
      if(zOTTxLVCwN == true){zOTTxLVCwN = false;}
      if(kybLGGRsNN == true){kybLGGRsNN = false;}
      if(NMLBOZfttg == true){NMLBOZfttg = false;}
      if(gNxTxIuEda == true){gNxTxIuEda = false;}
      if(zhhfssOArB == true){zhhfssOArB = false;}
      if(jiutKmCSEX == true){jiutKmCSEX = false;}
      if(eaIPfWAIOx == true){eaIPfWAIOx = false;}
      if(LMPlOfdfHV == true){LMPlOfdfHV = false;}
      if(LfCswxRDIK == true){LfCswxRDIK = false;}
      if(GcAGwXZSzV == true){GcAGwXZSzV = false;}
      if(sFktoWTJfe == true){sFktoWTJfe = false;}
      if(IVKiLzXOSi == true){IVKiLzXOSi = false;}
      if(FxzEqiMJpp == true){FxzEqiMJpp = false;}
      if(GLglbFtIDY == true){GLglbFtIDY = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FCBUMUGMCA
{ 
  void mWMsEOqZcB()
  { 
      bool ewpVkwhpWi = false;
      bool hBCxldXrlw = false;
      bool jXplojMjtL = false;
      bool tAEcGhMjZo = false;
      bool mTTrjNbkde = false;
      bool VRJbHGFDgB = false;
      bool ixuLogwZpD = false;
      bool RmRxJPEyCb = false;
      bool HbGOpbpcUA = false;
      bool cIDYFoYICD = false;
      bool QCtZNizMSq = false;
      bool CHBFtHpfUM = false;
      bool pTEpaIisrT = false;
      bool qwuyKEggBG = false;
      bool bdKcxVHydy = false;
      bool yrmpMwyEyF = false;
      bool zGUyAFLCto = false;
      bool ZJeOkLJarF = false;
      bool AkOtVzJFlN = false;
      bool GchwHVJLcq = false;
      string wJZhKOqJQX;
      string bubauesbEc;
      string JDzYiSLAky;
      string HdFzfmswSX;
      string uoIXEmZaVu;
      string bVyiosKAyr;
      string DqXSPyobtH;
      string kwIghUtbUw;
      string rjepWkeWJl;
      string csyaTEQpHG;
      string yxfNJeMQBU;
      string zifCzSLhud;
      string SSSIeryhMd;
      string DETOytDQgq;
      string YOUomsNuaB;
      string ritcmfeXiQ;
      string geEXQtOREE;
      string PuAEFExcUk;
      string eCGLkUKqaR;
      string PehPJsKwXr;
      if(wJZhKOqJQX == yxfNJeMQBU){ewpVkwhpWi = true;}
      else if(yxfNJeMQBU == wJZhKOqJQX){QCtZNizMSq = true;}
      if(bubauesbEc == zifCzSLhud){hBCxldXrlw = true;}
      else if(zifCzSLhud == bubauesbEc){CHBFtHpfUM = true;}
      if(JDzYiSLAky == SSSIeryhMd){jXplojMjtL = true;}
      else if(SSSIeryhMd == JDzYiSLAky){pTEpaIisrT = true;}
      if(HdFzfmswSX == DETOytDQgq){tAEcGhMjZo = true;}
      else if(DETOytDQgq == HdFzfmswSX){qwuyKEggBG = true;}
      if(uoIXEmZaVu == YOUomsNuaB){mTTrjNbkde = true;}
      else if(YOUomsNuaB == uoIXEmZaVu){bdKcxVHydy = true;}
      if(bVyiosKAyr == ritcmfeXiQ){VRJbHGFDgB = true;}
      else if(ritcmfeXiQ == bVyiosKAyr){yrmpMwyEyF = true;}
      if(DqXSPyobtH == geEXQtOREE){ixuLogwZpD = true;}
      else if(geEXQtOREE == DqXSPyobtH){zGUyAFLCto = true;}
      if(kwIghUtbUw == PuAEFExcUk){RmRxJPEyCb = true;}
      if(rjepWkeWJl == eCGLkUKqaR){HbGOpbpcUA = true;}
      if(csyaTEQpHG == PehPJsKwXr){cIDYFoYICD = true;}
      while(PuAEFExcUk == kwIghUtbUw){ZJeOkLJarF = true;}
      while(eCGLkUKqaR == eCGLkUKqaR){AkOtVzJFlN = true;}
      while(PehPJsKwXr == PehPJsKwXr){GchwHVJLcq = true;}
      if(ewpVkwhpWi == true){ewpVkwhpWi = false;}
      if(hBCxldXrlw == true){hBCxldXrlw = false;}
      if(jXplojMjtL == true){jXplojMjtL = false;}
      if(tAEcGhMjZo == true){tAEcGhMjZo = false;}
      if(mTTrjNbkde == true){mTTrjNbkde = false;}
      if(VRJbHGFDgB == true){VRJbHGFDgB = false;}
      if(ixuLogwZpD == true){ixuLogwZpD = false;}
      if(RmRxJPEyCb == true){RmRxJPEyCb = false;}
      if(HbGOpbpcUA == true){HbGOpbpcUA = false;}
      if(cIDYFoYICD == true){cIDYFoYICD = false;}
      if(QCtZNizMSq == true){QCtZNizMSq = false;}
      if(CHBFtHpfUM == true){CHBFtHpfUM = false;}
      if(pTEpaIisrT == true){pTEpaIisrT = false;}
      if(qwuyKEggBG == true){qwuyKEggBG = false;}
      if(bdKcxVHydy == true){bdKcxVHydy = false;}
      if(yrmpMwyEyF == true){yrmpMwyEyF = false;}
      if(zGUyAFLCto == true){zGUyAFLCto = false;}
      if(ZJeOkLJarF == true){ZJeOkLJarF = false;}
      if(AkOtVzJFlN == true){AkOtVzJFlN = false;}
      if(GchwHVJLcq == true){GchwHVJLcq = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KRIOMUAOVB
{ 
  void joBKiMXZpy()
  { 
      bool NVTiHVDUNA = false;
      bool rBJPZHeSaH = false;
      bool ZUBXBarbtk = false;
      bool ysekKMPWFV = false;
      bool YTTKDVVLAe = false;
      bool SRJyrsTuhg = false;
      bool caZbCzWYFN = false;
      bool rHMEiFAqUf = false;
      bool EfbORfigrQ = false;
      bool rToYrxsqIE = false;
      bool ywGlmLEEPm = false;
      bool scaCbxzKKV = false;
      bool PpfnUXSnMk = false;
      bool LIhPgHZobG = false;
      bool bdQFXOsmwu = false;
      bool xigloIQRQO = false;
      bool rPrdajtKUT = false;
      bool WJiDtkYaQG = false;
      bool efrRmcbwaX = false;
      bool qIXNNLlmdm = false;
      string macDeBelKh;
      string yioVBnZCtD;
      string OaEozWKErE;
      string iYCFyRecyE;
      string ymUMJgyTLi;
      string LCyCuLWmDH;
      string patFyZfjWi;
      string JwEpEbGqeS;
      string YprIsgZAJl;
      string uqYpNXrONj;
      string BNxcZOyIyT;
      string FeIkizilMR;
      string AeajnRWNbf;
      string iqcuQKsPie;
      string iRCpgPinHO;
      string oGCwiVmNnI;
      string qkfRjOBNTY;
      string GTChiickDY;
      string hiFNqcngLu;
      string WfaZmykjFR;
      if(macDeBelKh == BNxcZOyIyT){NVTiHVDUNA = true;}
      else if(BNxcZOyIyT == macDeBelKh){ywGlmLEEPm = true;}
      if(yioVBnZCtD == FeIkizilMR){rBJPZHeSaH = true;}
      else if(FeIkizilMR == yioVBnZCtD){scaCbxzKKV = true;}
      if(OaEozWKErE == AeajnRWNbf){ZUBXBarbtk = true;}
      else if(AeajnRWNbf == OaEozWKErE){PpfnUXSnMk = true;}
      if(iYCFyRecyE == iqcuQKsPie){ysekKMPWFV = true;}
      else if(iqcuQKsPie == iYCFyRecyE){LIhPgHZobG = true;}
      if(ymUMJgyTLi == iRCpgPinHO){YTTKDVVLAe = true;}
      else if(iRCpgPinHO == ymUMJgyTLi){bdQFXOsmwu = true;}
      if(LCyCuLWmDH == oGCwiVmNnI){SRJyrsTuhg = true;}
      else if(oGCwiVmNnI == LCyCuLWmDH){xigloIQRQO = true;}
      if(patFyZfjWi == qkfRjOBNTY){caZbCzWYFN = true;}
      else if(qkfRjOBNTY == patFyZfjWi){rPrdajtKUT = true;}
      if(JwEpEbGqeS == GTChiickDY){rHMEiFAqUf = true;}
      if(YprIsgZAJl == hiFNqcngLu){EfbORfigrQ = true;}
      if(uqYpNXrONj == WfaZmykjFR){rToYrxsqIE = true;}
      while(GTChiickDY == JwEpEbGqeS){WJiDtkYaQG = true;}
      while(hiFNqcngLu == hiFNqcngLu){efrRmcbwaX = true;}
      while(WfaZmykjFR == WfaZmykjFR){qIXNNLlmdm = true;}
      if(NVTiHVDUNA == true){NVTiHVDUNA = false;}
      if(rBJPZHeSaH == true){rBJPZHeSaH = false;}
      if(ZUBXBarbtk == true){ZUBXBarbtk = false;}
      if(ysekKMPWFV == true){ysekKMPWFV = false;}
      if(YTTKDVVLAe == true){YTTKDVVLAe = false;}
      if(SRJyrsTuhg == true){SRJyrsTuhg = false;}
      if(caZbCzWYFN == true){caZbCzWYFN = false;}
      if(rHMEiFAqUf == true){rHMEiFAqUf = false;}
      if(EfbORfigrQ == true){EfbORfigrQ = false;}
      if(rToYrxsqIE == true){rToYrxsqIE = false;}
      if(ywGlmLEEPm == true){ywGlmLEEPm = false;}
      if(scaCbxzKKV == true){scaCbxzKKV = false;}
      if(PpfnUXSnMk == true){PpfnUXSnMk = false;}
      if(LIhPgHZobG == true){LIhPgHZobG = false;}
      if(bdQFXOsmwu == true){bdQFXOsmwu = false;}
      if(xigloIQRQO == true){xigloIQRQO = false;}
      if(rPrdajtKUT == true){rPrdajtKUT = false;}
      if(WJiDtkYaQG == true){WJiDtkYaQG = false;}
      if(efrRmcbwaX == true){efrRmcbwaX = false;}
      if(qIXNNLlmdm == true){qIXNNLlmdm = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QAMXNTRVSO
{ 
  void YJCModuAKm()
  { 
      bool cYoGqpwYwx = false;
      bool pLUesAcscI = false;
      bool RYreldBUmk = false;
      bool uqZzEqpUCR = false;
      bool ynpYzuGAyF = false;
      bool hpoYQEgJUN = false;
      bool oakLBzlUaB = false;
      bool JIpLJchSep = false;
      bool XAVhCFBAiA = false;
      bool KfAocXDeWA = false;
      bool JkZxuYzdPm = false;
      bool VNedpnEDIw = false;
      bool xklapdIBil = false;
      bool ZDwdglaYld = false;
      bool GXkwdxBVyn = false;
      bool SzVZDYzzGR = false;
      bool QxtdMRQAxD = false;
      bool NfQdpZzows = false;
      bool blCtSZhVaH = false;
      bool maRUNIcHiL = false;
      string wSJLuNULca;
      string HHNdBBsVSq;
      string aKtPymyNip;
      string TDWaJBuhbu;
      string IAjLjwzTjT;
      string hYCBhkXtMI;
      string OGcmGEjlUz;
      string FLtXhaaifC;
      string nedOItBVsX;
      string cBUTgBWwEf;
      string MMzjNVedSH;
      string JoOKTjcNeL;
      string CNswVEpZqg;
      string DedzoaetBl;
      string PWDkNnmKzw;
      string YIRrczTDNJ;
      string fEmkDeKOUs;
      string FhMsweJHhh;
      string QfatIuMlWp;
      string HafqFLiShh;
      if(wSJLuNULca == MMzjNVedSH){cYoGqpwYwx = true;}
      else if(MMzjNVedSH == wSJLuNULca){JkZxuYzdPm = true;}
      if(HHNdBBsVSq == JoOKTjcNeL){pLUesAcscI = true;}
      else if(JoOKTjcNeL == HHNdBBsVSq){VNedpnEDIw = true;}
      if(aKtPymyNip == CNswVEpZqg){RYreldBUmk = true;}
      else if(CNswVEpZqg == aKtPymyNip){xklapdIBil = true;}
      if(TDWaJBuhbu == DedzoaetBl){uqZzEqpUCR = true;}
      else if(DedzoaetBl == TDWaJBuhbu){ZDwdglaYld = true;}
      if(IAjLjwzTjT == PWDkNnmKzw){ynpYzuGAyF = true;}
      else if(PWDkNnmKzw == IAjLjwzTjT){GXkwdxBVyn = true;}
      if(hYCBhkXtMI == YIRrczTDNJ){hpoYQEgJUN = true;}
      else if(YIRrczTDNJ == hYCBhkXtMI){SzVZDYzzGR = true;}
      if(OGcmGEjlUz == fEmkDeKOUs){oakLBzlUaB = true;}
      else if(fEmkDeKOUs == OGcmGEjlUz){QxtdMRQAxD = true;}
      if(FLtXhaaifC == FhMsweJHhh){JIpLJchSep = true;}
      if(nedOItBVsX == QfatIuMlWp){XAVhCFBAiA = true;}
      if(cBUTgBWwEf == HafqFLiShh){KfAocXDeWA = true;}
      while(FhMsweJHhh == FLtXhaaifC){NfQdpZzows = true;}
      while(QfatIuMlWp == QfatIuMlWp){blCtSZhVaH = true;}
      while(HafqFLiShh == HafqFLiShh){maRUNIcHiL = true;}
      if(cYoGqpwYwx == true){cYoGqpwYwx = false;}
      if(pLUesAcscI == true){pLUesAcscI = false;}
      if(RYreldBUmk == true){RYreldBUmk = false;}
      if(uqZzEqpUCR == true){uqZzEqpUCR = false;}
      if(ynpYzuGAyF == true){ynpYzuGAyF = false;}
      if(hpoYQEgJUN == true){hpoYQEgJUN = false;}
      if(oakLBzlUaB == true){oakLBzlUaB = false;}
      if(JIpLJchSep == true){JIpLJchSep = false;}
      if(XAVhCFBAiA == true){XAVhCFBAiA = false;}
      if(KfAocXDeWA == true){KfAocXDeWA = false;}
      if(JkZxuYzdPm == true){JkZxuYzdPm = false;}
      if(VNedpnEDIw == true){VNedpnEDIw = false;}
      if(xklapdIBil == true){xklapdIBil = false;}
      if(ZDwdglaYld == true){ZDwdglaYld = false;}
      if(GXkwdxBVyn == true){GXkwdxBVyn = false;}
      if(SzVZDYzzGR == true){SzVZDYzzGR = false;}
      if(QxtdMRQAxD == true){QxtdMRQAxD = false;}
      if(NfQdpZzows == true){NfQdpZzows = false;}
      if(blCtSZhVaH == true){blCtSZhVaH = false;}
      if(maRUNIcHiL == true){maRUNIcHiL = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class HVRWJMAAJN
{ 
  void HkWqsQEMjU()
  { 
      bool uZAKbXnJxn = false;
      bool OVgtRiGfZr = false;
      bool CrRHngCZHI = false;
      bool WDEslCmNbU = false;
      bool gbLIZMiOAK = false;
      bool PPIIXCHsWt = false;
      bool UOBoRKQmHP = false;
      bool wrDELZgIhb = false;
      bool ynYxsHuwVi = false;
      bool ttqgzRaVqO = false;
      bool uuGkOWLDwx = false;
      bool VyKtwgoEwJ = false;
      bool PTRYUBBVEt = false;
      bool zKRSfDgmiW = false;
      bool uqAXNuiwkS = false;
      bool bWZgzadhQM = false;
      bool VoyBxKgrOs = false;
      bool ACUZOXTsKY = false;
      bool SEklRckjSE = false;
      bool YyWVAFryaC = false;
      string OFsGokweuU;
      string ZZcCdJNXSY;
      string xzYOtBWJEC;
      string yctdQPltCB;
      string RLXeDStVTh;
      string bIgAAOfdMm;
      string NlasVwfylN;
      string XSVRHlbtLb;
      string GApVZowsdC;
      string iHhOmekdIG;
      string lwjFJImPsO;
      string llSkgNwESg;
      string zlUidQkGdb;
      string BxkEoKQjLQ;
      string CgAOPwnxhZ;
      string ifLbHAbAas;
      string XfkRNoBOwW;
      string xLrBdSlFTg;
      string bnYErUlIxn;
      string hPISsVwhAa;
      if(OFsGokweuU == lwjFJImPsO){uZAKbXnJxn = true;}
      else if(lwjFJImPsO == OFsGokweuU){uuGkOWLDwx = true;}
      if(ZZcCdJNXSY == llSkgNwESg){OVgtRiGfZr = true;}
      else if(llSkgNwESg == ZZcCdJNXSY){VyKtwgoEwJ = true;}
      if(xzYOtBWJEC == zlUidQkGdb){CrRHngCZHI = true;}
      else if(zlUidQkGdb == xzYOtBWJEC){PTRYUBBVEt = true;}
      if(yctdQPltCB == BxkEoKQjLQ){WDEslCmNbU = true;}
      else if(BxkEoKQjLQ == yctdQPltCB){zKRSfDgmiW = true;}
      if(RLXeDStVTh == CgAOPwnxhZ){gbLIZMiOAK = true;}
      else if(CgAOPwnxhZ == RLXeDStVTh){uqAXNuiwkS = true;}
      if(bIgAAOfdMm == ifLbHAbAas){PPIIXCHsWt = true;}
      else if(ifLbHAbAas == bIgAAOfdMm){bWZgzadhQM = true;}
      if(NlasVwfylN == XfkRNoBOwW){UOBoRKQmHP = true;}
      else if(XfkRNoBOwW == NlasVwfylN){VoyBxKgrOs = true;}
      if(XSVRHlbtLb == xLrBdSlFTg){wrDELZgIhb = true;}
      if(GApVZowsdC == bnYErUlIxn){ynYxsHuwVi = true;}
      if(iHhOmekdIG == hPISsVwhAa){ttqgzRaVqO = true;}
      while(xLrBdSlFTg == XSVRHlbtLb){ACUZOXTsKY = true;}
      while(bnYErUlIxn == bnYErUlIxn){SEklRckjSE = true;}
      while(hPISsVwhAa == hPISsVwhAa){YyWVAFryaC = true;}
      if(uZAKbXnJxn == true){uZAKbXnJxn = false;}
      if(OVgtRiGfZr == true){OVgtRiGfZr = false;}
      if(CrRHngCZHI == true){CrRHngCZHI = false;}
      if(WDEslCmNbU == true){WDEslCmNbU = false;}
      if(gbLIZMiOAK == true){gbLIZMiOAK = false;}
      if(PPIIXCHsWt == true){PPIIXCHsWt = false;}
      if(UOBoRKQmHP == true){UOBoRKQmHP = false;}
      if(wrDELZgIhb == true){wrDELZgIhb = false;}
      if(ynYxsHuwVi == true){ynYxsHuwVi = false;}
      if(ttqgzRaVqO == true){ttqgzRaVqO = false;}
      if(uuGkOWLDwx == true){uuGkOWLDwx = false;}
      if(VyKtwgoEwJ == true){VyKtwgoEwJ = false;}
      if(PTRYUBBVEt == true){PTRYUBBVEt = false;}
      if(zKRSfDgmiW == true){zKRSfDgmiW = false;}
      if(uqAXNuiwkS == true){uqAXNuiwkS = false;}
      if(bWZgzadhQM == true){bWZgzadhQM = false;}
      if(VoyBxKgrOs == true){VoyBxKgrOs = false;}
      if(ACUZOXTsKY == true){ACUZOXTsKY = false;}
      if(SEklRckjSE == true){SEklRckjSE = false;}
      if(YyWVAFryaC == true){YyWVAFryaC = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZBOPEHFFTA
{ 
  void BRNaerGdRB()
  { 
      bool ZGiskGgAGm = false;
      bool EgGHeouRlt = false;
      bool RXQbmJnGsw = false;
      bool EuPzGlVTQk = false;
      bool uOQZJOLxRi = false;
      bool LMFogCoHFa = false;
      bool RZGJhIQGAs = false;
      bool SaABzCGqSY = false;
      bool ypnKkqpaSE = false;
      bool wNOpSUIGqA = false;
      bool xEwXPjMEpd = false;
      bool uynfbbkwYq = false;
      bool SpmwxFMJhd = false;
      bool tzzRassQtB = false;
      bool dkjnIxZhNq = false;
      bool iLhAhHRAaK = false;
      bool WCVMjuyVFi = false;
      bool hUBXMgFzmz = false;
      bool eduqthRzjT = false;
      bool bsojdaPXEM = false;
      string GtAwiKujpS;
      string DZuMeOVymg;
      string bJLCwEIZpQ;
      string nofyperUTN;
      string GEbEQslEpV;
      string ZbIsHwhxzd;
      string CVsZdNtxWz;
      string WAdZZXhGkD;
      string RDjTqMgmps;
      string wuVoKjKBaD;
      string ynhqzYxhSj;
      string NaLuMykYiR;
      string gHHcCbeoOp;
      string MgfhFcVsUR;
      string OyTGeUiZbM;
      string ykRIVqgEnX;
      string tITcWgucpi;
      string brERCimAyg;
      string rFMBqbPWMN;
      string uRExXGeqjy;
      if(GtAwiKujpS == ynhqzYxhSj){ZGiskGgAGm = true;}
      else if(ynhqzYxhSj == GtAwiKujpS){xEwXPjMEpd = true;}
      if(DZuMeOVymg == NaLuMykYiR){EgGHeouRlt = true;}
      else if(NaLuMykYiR == DZuMeOVymg){uynfbbkwYq = true;}
      if(bJLCwEIZpQ == gHHcCbeoOp){RXQbmJnGsw = true;}
      else if(gHHcCbeoOp == bJLCwEIZpQ){SpmwxFMJhd = true;}
      if(nofyperUTN == MgfhFcVsUR){EuPzGlVTQk = true;}
      else if(MgfhFcVsUR == nofyperUTN){tzzRassQtB = true;}
      if(GEbEQslEpV == OyTGeUiZbM){uOQZJOLxRi = true;}
      else if(OyTGeUiZbM == GEbEQslEpV){dkjnIxZhNq = true;}
      if(ZbIsHwhxzd == ykRIVqgEnX){LMFogCoHFa = true;}
      else if(ykRIVqgEnX == ZbIsHwhxzd){iLhAhHRAaK = true;}
      if(CVsZdNtxWz == tITcWgucpi){RZGJhIQGAs = true;}
      else if(tITcWgucpi == CVsZdNtxWz){WCVMjuyVFi = true;}
      if(WAdZZXhGkD == brERCimAyg){SaABzCGqSY = true;}
      if(RDjTqMgmps == rFMBqbPWMN){ypnKkqpaSE = true;}
      if(wuVoKjKBaD == uRExXGeqjy){wNOpSUIGqA = true;}
      while(brERCimAyg == WAdZZXhGkD){hUBXMgFzmz = true;}
      while(rFMBqbPWMN == rFMBqbPWMN){eduqthRzjT = true;}
      while(uRExXGeqjy == uRExXGeqjy){bsojdaPXEM = true;}
      if(ZGiskGgAGm == true){ZGiskGgAGm = false;}
      if(EgGHeouRlt == true){EgGHeouRlt = false;}
      if(RXQbmJnGsw == true){RXQbmJnGsw = false;}
      if(EuPzGlVTQk == true){EuPzGlVTQk = false;}
      if(uOQZJOLxRi == true){uOQZJOLxRi = false;}
      if(LMFogCoHFa == true){LMFogCoHFa = false;}
      if(RZGJhIQGAs == true){RZGJhIQGAs = false;}
      if(SaABzCGqSY == true){SaABzCGqSY = false;}
      if(ypnKkqpaSE == true){ypnKkqpaSE = false;}
      if(wNOpSUIGqA == true){wNOpSUIGqA = false;}
      if(xEwXPjMEpd == true){xEwXPjMEpd = false;}
      if(uynfbbkwYq == true){uynfbbkwYq = false;}
      if(SpmwxFMJhd == true){SpmwxFMJhd = false;}
      if(tzzRassQtB == true){tzzRassQtB = false;}
      if(dkjnIxZhNq == true){dkjnIxZhNq = false;}
      if(iLhAhHRAaK == true){iLhAhHRAaK = false;}
      if(WCVMjuyVFi == true){WCVMjuyVFi = false;}
      if(hUBXMgFzmz == true){hUBXMgFzmz = false;}
      if(eduqthRzjT == true){eduqthRzjT = false;}
      if(bsojdaPXEM == true){bsojdaPXEM = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ADLAOEGBYF
{ 
  void qQiosJVtiM()
  { 
      bool DlAmFinKIX = false;
      bool MbywASjSaM = false;
      bool YmQtNWRFas = false;
      bool MNCIltOBVh = false;
      bool aRAleeSdWV = false;
      bool RjCOQQUGYi = false;
      bool QhjOLyZoJF = false;
      bool DhxrlHmeOh = false;
      bool DmhFabLmrV = false;
      bool ZIblRrhlZg = false;
      bool FcURRjlJcI = false;
      bool doqpedzdmi = false;
      bool llxnftFLas = false;
      bool ZAEgcaSQmq = false;
      bool pCKiqjRlGm = false;
      bool rDyquSefkW = false;
      bool dahiicjJNE = false;
      bool KipusibOwu = false;
      bool cTEmAVCNlJ = false;
      bool umeQNOTaAO = false;
      string zXysRbKxHV;
      string QkkGCnhUKy;
      string rbcazeZQjL;
      string FpQuTDJNeu;
      string ZMJpblcZaE;
      string EKdJlWtXtl;
      string BtMuQZUpmy;
      string eUoOXeRlyM;
      string CAoeHuMHJw;
      string IkSrtfzOZB;
      string RmmcZjUOkG;
      string FIYgjBEhmV;
      string NNgppsBwUr;
      string fDMxuONMEX;
      string ZxVCqzMlIs;
      string aRaDmDGwlg;
      string hXtraBwakV;
      string mrdPuPqdyp;
      string fPnaRObIJq;
      string HQZXoBDcbp;
      if(zXysRbKxHV == RmmcZjUOkG){DlAmFinKIX = true;}
      else if(RmmcZjUOkG == zXysRbKxHV){FcURRjlJcI = true;}
      if(QkkGCnhUKy == FIYgjBEhmV){MbywASjSaM = true;}
      else if(FIYgjBEhmV == QkkGCnhUKy){doqpedzdmi = true;}
      if(rbcazeZQjL == NNgppsBwUr){YmQtNWRFas = true;}
      else if(NNgppsBwUr == rbcazeZQjL){llxnftFLas = true;}
      if(FpQuTDJNeu == fDMxuONMEX){MNCIltOBVh = true;}
      else if(fDMxuONMEX == FpQuTDJNeu){ZAEgcaSQmq = true;}
      if(ZMJpblcZaE == ZxVCqzMlIs){aRAleeSdWV = true;}
      else if(ZxVCqzMlIs == ZMJpblcZaE){pCKiqjRlGm = true;}
      if(EKdJlWtXtl == aRaDmDGwlg){RjCOQQUGYi = true;}
      else if(aRaDmDGwlg == EKdJlWtXtl){rDyquSefkW = true;}
      if(BtMuQZUpmy == hXtraBwakV){QhjOLyZoJF = true;}
      else if(hXtraBwakV == BtMuQZUpmy){dahiicjJNE = true;}
      if(eUoOXeRlyM == mrdPuPqdyp){DhxrlHmeOh = true;}
      if(CAoeHuMHJw == fPnaRObIJq){DmhFabLmrV = true;}
      if(IkSrtfzOZB == HQZXoBDcbp){ZIblRrhlZg = true;}
      while(mrdPuPqdyp == eUoOXeRlyM){KipusibOwu = true;}
      while(fPnaRObIJq == fPnaRObIJq){cTEmAVCNlJ = true;}
      while(HQZXoBDcbp == HQZXoBDcbp){umeQNOTaAO = true;}
      if(DlAmFinKIX == true){DlAmFinKIX = false;}
      if(MbywASjSaM == true){MbywASjSaM = false;}
      if(YmQtNWRFas == true){YmQtNWRFas = false;}
      if(MNCIltOBVh == true){MNCIltOBVh = false;}
      if(aRAleeSdWV == true){aRAleeSdWV = false;}
      if(RjCOQQUGYi == true){RjCOQQUGYi = false;}
      if(QhjOLyZoJF == true){QhjOLyZoJF = false;}
      if(DhxrlHmeOh == true){DhxrlHmeOh = false;}
      if(DmhFabLmrV == true){DmhFabLmrV = false;}
      if(ZIblRrhlZg == true){ZIblRrhlZg = false;}
      if(FcURRjlJcI == true){FcURRjlJcI = false;}
      if(doqpedzdmi == true){doqpedzdmi = false;}
      if(llxnftFLas == true){llxnftFLas = false;}
      if(ZAEgcaSQmq == true){ZAEgcaSQmq = false;}
      if(pCKiqjRlGm == true){pCKiqjRlGm = false;}
      if(rDyquSefkW == true){rDyquSefkW = false;}
      if(dahiicjJNE == true){dahiicjJNE = false;}
      if(KipusibOwu == true){KipusibOwu = false;}
      if(cTEmAVCNlJ == true){cTEmAVCNlJ = false;}
      if(umeQNOTaAO == true){umeQNOTaAO = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KKZZRFZDJM
{ 
  void DlXmLYSjDb()
  { 
      bool XBZXbsXMhl = false;
      bool BlUERPzjkQ = false;
      bool iJHTjDbYhc = false;
      bool bYZnAfotoH = false;
      bool mtadWNcCww = false;
      bool kCpsnOsFmX = false;
      bool RzdFJcMfDk = false;
      bool aFudcnidxW = false;
      bool ZZblrZhfAl = false;
      bool miHQpnQUTs = false;
      bool AxyLUeGMQz = false;
      bool eOIPxqWqNj = false;
      bool uffQyqydmo = false;
      bool XNKDWcaQRM = false;
      bool SVPAGYZqTy = false;
      bool QYHjyURhMc = false;
      bool fcYaaRDEsU = false;
      bool PGZAQYQeJZ = false;
      bool RGJnXzGaFB = false;
      bool LsDngpxnZo = false;
      string xUZMfGuIRc;
      string aMaadrMSgN;
      string NkhOBMnApe;
      string UlSTfAYufb;
      string odeSsKzCWY;
      string cbBnhLlKKS;
      string SuRlHnsGtd;
      string TRlEXiGPVq;
      string NlcRBrkekT;
      string NNkMJtLrKE;
      string QrHsELpwQz;
      string FWoSFfRAkP;
      string DOVaPFgYtc;
      string WOEQaVdjMp;
      string CnOlOpYRHd;
      string bLHuBCVgcy;
      string dRBmbqSXrj;
      string BhmtNOAjLY;
      string gORQBLHIPY;
      string zxGOLklbJX;
      if(xUZMfGuIRc == QrHsELpwQz){XBZXbsXMhl = true;}
      else if(QrHsELpwQz == xUZMfGuIRc){AxyLUeGMQz = true;}
      if(aMaadrMSgN == FWoSFfRAkP){BlUERPzjkQ = true;}
      else if(FWoSFfRAkP == aMaadrMSgN){eOIPxqWqNj = true;}
      if(NkhOBMnApe == DOVaPFgYtc){iJHTjDbYhc = true;}
      else if(DOVaPFgYtc == NkhOBMnApe){uffQyqydmo = true;}
      if(UlSTfAYufb == WOEQaVdjMp){bYZnAfotoH = true;}
      else if(WOEQaVdjMp == UlSTfAYufb){XNKDWcaQRM = true;}
      if(odeSsKzCWY == CnOlOpYRHd){mtadWNcCww = true;}
      else if(CnOlOpYRHd == odeSsKzCWY){SVPAGYZqTy = true;}
      if(cbBnhLlKKS == bLHuBCVgcy){kCpsnOsFmX = true;}
      else if(bLHuBCVgcy == cbBnhLlKKS){QYHjyURhMc = true;}
      if(SuRlHnsGtd == dRBmbqSXrj){RzdFJcMfDk = true;}
      else if(dRBmbqSXrj == SuRlHnsGtd){fcYaaRDEsU = true;}
      if(TRlEXiGPVq == BhmtNOAjLY){aFudcnidxW = true;}
      if(NlcRBrkekT == gORQBLHIPY){ZZblrZhfAl = true;}
      if(NNkMJtLrKE == zxGOLklbJX){miHQpnQUTs = true;}
      while(BhmtNOAjLY == TRlEXiGPVq){PGZAQYQeJZ = true;}
      while(gORQBLHIPY == gORQBLHIPY){RGJnXzGaFB = true;}
      while(zxGOLklbJX == zxGOLklbJX){LsDngpxnZo = true;}
      if(XBZXbsXMhl == true){XBZXbsXMhl = false;}
      if(BlUERPzjkQ == true){BlUERPzjkQ = false;}
      if(iJHTjDbYhc == true){iJHTjDbYhc = false;}
      if(bYZnAfotoH == true){bYZnAfotoH = false;}
      if(mtadWNcCww == true){mtadWNcCww = false;}
      if(kCpsnOsFmX == true){kCpsnOsFmX = false;}
      if(RzdFJcMfDk == true){RzdFJcMfDk = false;}
      if(aFudcnidxW == true){aFudcnidxW = false;}
      if(ZZblrZhfAl == true){ZZblrZhfAl = false;}
      if(miHQpnQUTs == true){miHQpnQUTs = false;}
      if(AxyLUeGMQz == true){AxyLUeGMQz = false;}
      if(eOIPxqWqNj == true){eOIPxqWqNj = false;}
      if(uffQyqydmo == true){uffQyqydmo = false;}
      if(XNKDWcaQRM == true){XNKDWcaQRM = false;}
      if(SVPAGYZqTy == true){SVPAGYZqTy = false;}
      if(QYHjyURhMc == true){QYHjyURhMc = false;}
      if(fcYaaRDEsU == true){fcYaaRDEsU = false;}
      if(PGZAQYQeJZ == true){PGZAQYQeJZ = false;}
      if(RGJnXzGaFB == true){RGJnXzGaFB = false;}
      if(LsDngpxnZo == true){LsDngpxnZo = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UBVCWNPYNG
{ 
  void dclMiEoLhl()
  { 
      bool rJBlYSeVzu = false;
      bool ZdbbiSwEAC = false;
      bool KmPWjdlCYa = false;
      bool KWmZsRUYde = false;
      bool BUiTLFhVpe = false;
      bool eKtepEseCt = false;
      bool gcxXkCpChQ = false;
      bool yXZVAoDuKe = false;
      bool wPLtwzcIin = false;
      bool XoWiyYXLiI = false;
      bool OfmYfHkYdB = false;
      bool ByTXxQRDbk = false;
      bool ZhSssIApWb = false;
      bool RqMBPQclNn = false;
      bool NDdhmZUzlg = false;
      bool GTjxigMsfW = false;
      bool GtBCiJkSHg = false;
      bool neomWWDjGT = false;
      bool nBEpUxHOSh = false;
      bool EiGqgTCazc = false;
      string erioolTTUA;
      string wdqeTojtqH;
      string qGKQaYqCAD;
      string ECfSMBVJWK;
      string MkTkMyIyer;
      string uwFqdYgSNQ;
      string uQTRutyYRS;
      string SaWftFlZDc;
      string PsWIHIwonn;
      string TDuGPuRCoS;
      string AifMYUYYwX;
      string lGOkABtKJm;
      string xJkczPfQQz;
      string njkjcgMKim;
      string jXrQCNnJFN;
      string BGrCUqyCNJ;
      string oHHxiBVeIS;
      string niaWDZQTak;
      string aVLDxgqiOl;
      string iJxYybPtgA;
      if(erioolTTUA == AifMYUYYwX){rJBlYSeVzu = true;}
      else if(AifMYUYYwX == erioolTTUA){OfmYfHkYdB = true;}
      if(wdqeTojtqH == lGOkABtKJm){ZdbbiSwEAC = true;}
      else if(lGOkABtKJm == wdqeTojtqH){ByTXxQRDbk = true;}
      if(qGKQaYqCAD == xJkczPfQQz){KmPWjdlCYa = true;}
      else if(xJkczPfQQz == qGKQaYqCAD){ZhSssIApWb = true;}
      if(ECfSMBVJWK == njkjcgMKim){KWmZsRUYde = true;}
      else if(njkjcgMKim == ECfSMBVJWK){RqMBPQclNn = true;}
      if(MkTkMyIyer == jXrQCNnJFN){BUiTLFhVpe = true;}
      else if(jXrQCNnJFN == MkTkMyIyer){NDdhmZUzlg = true;}
      if(uwFqdYgSNQ == BGrCUqyCNJ){eKtepEseCt = true;}
      else if(BGrCUqyCNJ == uwFqdYgSNQ){GTjxigMsfW = true;}
      if(uQTRutyYRS == oHHxiBVeIS){gcxXkCpChQ = true;}
      else if(oHHxiBVeIS == uQTRutyYRS){GtBCiJkSHg = true;}
      if(SaWftFlZDc == niaWDZQTak){yXZVAoDuKe = true;}
      if(PsWIHIwonn == aVLDxgqiOl){wPLtwzcIin = true;}
      if(TDuGPuRCoS == iJxYybPtgA){XoWiyYXLiI = true;}
      while(niaWDZQTak == SaWftFlZDc){neomWWDjGT = true;}
      while(aVLDxgqiOl == aVLDxgqiOl){nBEpUxHOSh = true;}
      while(iJxYybPtgA == iJxYybPtgA){EiGqgTCazc = true;}
      if(rJBlYSeVzu == true){rJBlYSeVzu = false;}
      if(ZdbbiSwEAC == true){ZdbbiSwEAC = false;}
      if(KmPWjdlCYa == true){KmPWjdlCYa = false;}
      if(KWmZsRUYde == true){KWmZsRUYde = false;}
      if(BUiTLFhVpe == true){BUiTLFhVpe = false;}
      if(eKtepEseCt == true){eKtepEseCt = false;}
      if(gcxXkCpChQ == true){gcxXkCpChQ = false;}
      if(yXZVAoDuKe == true){yXZVAoDuKe = false;}
      if(wPLtwzcIin == true){wPLtwzcIin = false;}
      if(XoWiyYXLiI == true){XoWiyYXLiI = false;}
      if(OfmYfHkYdB == true){OfmYfHkYdB = false;}
      if(ByTXxQRDbk == true){ByTXxQRDbk = false;}
      if(ZhSssIApWb == true){ZhSssIApWb = false;}
      if(RqMBPQclNn == true){RqMBPQclNn = false;}
      if(NDdhmZUzlg == true){NDdhmZUzlg = false;}
      if(GTjxigMsfW == true){GTjxigMsfW = false;}
      if(GtBCiJkSHg == true){GtBCiJkSHg = false;}
      if(neomWWDjGT == true){neomWWDjGT = false;}
      if(nBEpUxHOSh == true){nBEpUxHOSh = false;}
      if(EiGqgTCazc == true){EiGqgTCazc = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZCZPQMYZXE
{ 
  void aNXjMEpBEU()
  { 
      bool GVrGajloJK = false;
      bool rLkYqOUNHH = false;
      bool uJAcUgUkoE = false;
      bool SNCXloPqzx = false;
      bool rVYSHNMeTy = false;
      bool ArCImxUPls = false;
      bool OAzEqdINmB = false;
      bool ycOJPZjgVG = false;
      bool UpRqDaLsjo = false;
      bool UAFFiqVUbc = false;
      bool EyrRAXcssN = false;
      bool MmseYYiHAP = false;
      bool SOcygxWJaY = false;
      bool DPFrBWxzqf = false;
      bool ySAVbgGagJ = false;
      bool INTLJoUtjC = false;
      bool ATRUKwhBcB = false;
      bool biQURAOHVA = false;
      bool pucQPowshM = false;
      bool BCOneeAfLe = false;
      string KSKplxTeuw;
      string aPlSpFzBee;
      string maoBuhBrsZ;
      string YrLLdnBICW;
      string gfxWkqcaMR;
      string pQceHjdIuD;
      string ublUWhDanC;
      string DpyIxHcSQD;
      string FafaiHPgDt;
      string KrNyaFsbzC;
      string mBlJNASUgA;
      string GKrryUuKkP;
      string SWWtrLczgr;
      string NBSNPDoVNi;
      string otzDnzAUAG;
      string JiqjjPeqRC;
      string WTnolFrebt;
      string eZwBaYXTGs;
      string KIyZhdKidz;
      string YxELMVFJQh;
      if(KSKplxTeuw == mBlJNASUgA){GVrGajloJK = true;}
      else if(mBlJNASUgA == KSKplxTeuw){EyrRAXcssN = true;}
      if(aPlSpFzBee == GKrryUuKkP){rLkYqOUNHH = true;}
      else if(GKrryUuKkP == aPlSpFzBee){MmseYYiHAP = true;}
      if(maoBuhBrsZ == SWWtrLczgr){uJAcUgUkoE = true;}
      else if(SWWtrLczgr == maoBuhBrsZ){SOcygxWJaY = true;}
      if(YrLLdnBICW == NBSNPDoVNi){SNCXloPqzx = true;}
      else if(NBSNPDoVNi == YrLLdnBICW){DPFrBWxzqf = true;}
      if(gfxWkqcaMR == otzDnzAUAG){rVYSHNMeTy = true;}
      else if(otzDnzAUAG == gfxWkqcaMR){ySAVbgGagJ = true;}
      if(pQceHjdIuD == JiqjjPeqRC){ArCImxUPls = true;}
      else if(JiqjjPeqRC == pQceHjdIuD){INTLJoUtjC = true;}
      if(ublUWhDanC == WTnolFrebt){OAzEqdINmB = true;}
      else if(WTnolFrebt == ublUWhDanC){ATRUKwhBcB = true;}
      if(DpyIxHcSQD == eZwBaYXTGs){ycOJPZjgVG = true;}
      if(FafaiHPgDt == KIyZhdKidz){UpRqDaLsjo = true;}
      if(KrNyaFsbzC == YxELMVFJQh){UAFFiqVUbc = true;}
      while(eZwBaYXTGs == DpyIxHcSQD){biQURAOHVA = true;}
      while(KIyZhdKidz == KIyZhdKidz){pucQPowshM = true;}
      while(YxELMVFJQh == YxELMVFJQh){BCOneeAfLe = true;}
      if(GVrGajloJK == true){GVrGajloJK = false;}
      if(rLkYqOUNHH == true){rLkYqOUNHH = false;}
      if(uJAcUgUkoE == true){uJAcUgUkoE = false;}
      if(SNCXloPqzx == true){SNCXloPqzx = false;}
      if(rVYSHNMeTy == true){rVYSHNMeTy = false;}
      if(ArCImxUPls == true){ArCImxUPls = false;}
      if(OAzEqdINmB == true){OAzEqdINmB = false;}
      if(ycOJPZjgVG == true){ycOJPZjgVG = false;}
      if(UpRqDaLsjo == true){UpRqDaLsjo = false;}
      if(UAFFiqVUbc == true){UAFFiqVUbc = false;}
      if(EyrRAXcssN == true){EyrRAXcssN = false;}
      if(MmseYYiHAP == true){MmseYYiHAP = false;}
      if(SOcygxWJaY == true){SOcygxWJaY = false;}
      if(DPFrBWxzqf == true){DPFrBWxzqf = false;}
      if(ySAVbgGagJ == true){ySAVbgGagJ = false;}
      if(INTLJoUtjC == true){INTLJoUtjC = false;}
      if(ATRUKwhBcB == true){ATRUKwhBcB = false;}
      if(biQURAOHVA == true){biQURAOHVA = false;}
      if(pucQPowshM == true){pucQPowshM = false;}
      if(BCOneeAfLe == true){BCOneeAfLe = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UYZKDQYXQB
{ 
  void lbYIMUCJeq()
  { 
      bool yfbyaescxw = false;
      bool NxsiEmFbrd = false;
      bool aJWgMwqrSs = false;
      bool OiqPdTDQDS = false;
      bool tiXNzNdTez = false;
      bool iMIpmnItke = false;
      bool nBJEheGdEj = false;
      bool jnYYirkIhy = false;
      bool SXMTJBZAbp = false;
      bool LJeYesYTfU = false;
      bool GzPuPtniHu = false;
      bool DzMlLPoptf = false;
      bool EGCOYZIztp = false;
      bool SQcMqSIQEP = false;
      bool NqsglLxsMO = false;
      bool DhZAPAxAVo = false;
      bool knoQblWztr = false;
      bool HXldRNNVbN = false;
      bool cPlelkqcwq = false;
      bool VHuOeYCbEA = false;
      string qrDfPRGPJf;
      string bQqOWlsrDm;
      string zUsuqsIOtX;
      string awhgDHPPnT;
      string cGfYgHQMgn;
      string xLRZTVzHnV;
      string JrVlEeiKLO;
      string VcEjKsoTNi;
      string bugGZPdHMg;
      string IHaRJkjtTB;
      string YGBRRUGhPa;
      string getFtOKPLo;
      string AaxdGNBlZY;
      string ItALKUtPnF;
      string IwqlAcNVXu;
      string aUidBxJWLE;
      string STHqRlehtY;
      string TubFgEiLcc;
      string AeOeynLVKN;
      string LXDimnoAUN;
      if(qrDfPRGPJf == YGBRRUGhPa){yfbyaescxw = true;}
      else if(YGBRRUGhPa == qrDfPRGPJf){GzPuPtniHu = true;}
      if(bQqOWlsrDm == getFtOKPLo){NxsiEmFbrd = true;}
      else if(getFtOKPLo == bQqOWlsrDm){DzMlLPoptf = true;}
      if(zUsuqsIOtX == AaxdGNBlZY){aJWgMwqrSs = true;}
      else if(AaxdGNBlZY == zUsuqsIOtX){EGCOYZIztp = true;}
      if(awhgDHPPnT == ItALKUtPnF){OiqPdTDQDS = true;}
      else if(ItALKUtPnF == awhgDHPPnT){SQcMqSIQEP = true;}
      if(cGfYgHQMgn == IwqlAcNVXu){tiXNzNdTez = true;}
      else if(IwqlAcNVXu == cGfYgHQMgn){NqsglLxsMO = true;}
      if(xLRZTVzHnV == aUidBxJWLE){iMIpmnItke = true;}
      else if(aUidBxJWLE == xLRZTVzHnV){DhZAPAxAVo = true;}
      if(JrVlEeiKLO == STHqRlehtY){nBJEheGdEj = true;}
      else if(STHqRlehtY == JrVlEeiKLO){knoQblWztr = true;}
      if(VcEjKsoTNi == TubFgEiLcc){jnYYirkIhy = true;}
      if(bugGZPdHMg == AeOeynLVKN){SXMTJBZAbp = true;}
      if(IHaRJkjtTB == LXDimnoAUN){LJeYesYTfU = true;}
      while(TubFgEiLcc == VcEjKsoTNi){HXldRNNVbN = true;}
      while(AeOeynLVKN == AeOeynLVKN){cPlelkqcwq = true;}
      while(LXDimnoAUN == LXDimnoAUN){VHuOeYCbEA = true;}
      if(yfbyaescxw == true){yfbyaescxw = false;}
      if(NxsiEmFbrd == true){NxsiEmFbrd = false;}
      if(aJWgMwqrSs == true){aJWgMwqrSs = false;}
      if(OiqPdTDQDS == true){OiqPdTDQDS = false;}
      if(tiXNzNdTez == true){tiXNzNdTez = false;}
      if(iMIpmnItke == true){iMIpmnItke = false;}
      if(nBJEheGdEj == true){nBJEheGdEj = false;}
      if(jnYYirkIhy == true){jnYYirkIhy = false;}
      if(SXMTJBZAbp == true){SXMTJBZAbp = false;}
      if(LJeYesYTfU == true){LJeYesYTfU = false;}
      if(GzPuPtniHu == true){GzPuPtniHu = false;}
      if(DzMlLPoptf == true){DzMlLPoptf = false;}
      if(EGCOYZIztp == true){EGCOYZIztp = false;}
      if(SQcMqSIQEP == true){SQcMqSIQEP = false;}
      if(NqsglLxsMO == true){NqsglLxsMO = false;}
      if(DhZAPAxAVo == true){DhZAPAxAVo = false;}
      if(knoQblWztr == true){knoQblWztr = false;}
      if(HXldRNNVbN == true){HXldRNNVbN = false;}
      if(cPlelkqcwq == true){cPlelkqcwq = false;}
      if(VHuOeYCbEA == true){VHuOeYCbEA = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class MFWDYKCDBN
{ 
  void fIOrywEaLX()
  { 
      bool cMKfiNlTFu = false;
      bool DISwRssNEe = false;
      bool ooVBMYcYEg = false;
      bool waCVzDlWri = false;
      bool IUdeiQGCwX = false;
      bool eKFXwnpITK = false;
      bool kMOYzcGxxO = false;
      bool HWUVVTKqSw = false;
      bool SZbfClUdYL = false;
      bool MdChxwGFfG = false;
      bool JIFhQHojBb = false;
      bool czoXrKkhWM = false;
      bool HcYlBcUmXZ = false;
      bool NFKLlJUtPt = false;
      bool wkbxhNndpm = false;
      bool KXhSygjSfm = false;
      bool mBLaNXnckh = false;
      bool opSbQWAbDn = false;
      bool opxkOpWrNF = false;
      bool YCNbHtYAiK = false;
      string ifJVJqFUEd;
      string ERKYXqASXt;
      string deeitwsefl;
      string QHTAbWWpFf;
      string QzjyuhItDb;
      string tesSaDBabM;
      string yaoSMxxJwA;
      string UKNrceugnL;
      string myaEqlOBZY;
      string VuOrhpKQky;
      string kyyDHjRAou;
      string ITlPZzzjbZ;
      string hwjXdYuUKm;
      string UdupanAXwG;
      string UNLdQBIzRh;
      string pZoLPnOaYh;
      string owpBadYwol;
      string ybmVFTjFHb;
      string PwCbxuojZm;
      string YZzOQXXJDl;
      if(ifJVJqFUEd == kyyDHjRAou){cMKfiNlTFu = true;}
      else if(kyyDHjRAou == ifJVJqFUEd){JIFhQHojBb = true;}
      if(ERKYXqASXt == ITlPZzzjbZ){DISwRssNEe = true;}
      else if(ITlPZzzjbZ == ERKYXqASXt){czoXrKkhWM = true;}
      if(deeitwsefl == hwjXdYuUKm){ooVBMYcYEg = true;}
      else if(hwjXdYuUKm == deeitwsefl){HcYlBcUmXZ = true;}
      if(QHTAbWWpFf == UdupanAXwG){waCVzDlWri = true;}
      else if(UdupanAXwG == QHTAbWWpFf){NFKLlJUtPt = true;}
      if(QzjyuhItDb == UNLdQBIzRh){IUdeiQGCwX = true;}
      else if(UNLdQBIzRh == QzjyuhItDb){wkbxhNndpm = true;}
      if(tesSaDBabM == pZoLPnOaYh){eKFXwnpITK = true;}
      else if(pZoLPnOaYh == tesSaDBabM){KXhSygjSfm = true;}
      if(yaoSMxxJwA == owpBadYwol){kMOYzcGxxO = true;}
      else if(owpBadYwol == yaoSMxxJwA){mBLaNXnckh = true;}
      if(UKNrceugnL == ybmVFTjFHb){HWUVVTKqSw = true;}
      if(myaEqlOBZY == PwCbxuojZm){SZbfClUdYL = true;}
      if(VuOrhpKQky == YZzOQXXJDl){MdChxwGFfG = true;}
      while(ybmVFTjFHb == UKNrceugnL){opSbQWAbDn = true;}
      while(PwCbxuojZm == PwCbxuojZm){opxkOpWrNF = true;}
      while(YZzOQXXJDl == YZzOQXXJDl){YCNbHtYAiK = true;}
      if(cMKfiNlTFu == true){cMKfiNlTFu = false;}
      if(DISwRssNEe == true){DISwRssNEe = false;}
      if(ooVBMYcYEg == true){ooVBMYcYEg = false;}
      if(waCVzDlWri == true){waCVzDlWri = false;}
      if(IUdeiQGCwX == true){IUdeiQGCwX = false;}
      if(eKFXwnpITK == true){eKFXwnpITK = false;}
      if(kMOYzcGxxO == true){kMOYzcGxxO = false;}
      if(HWUVVTKqSw == true){HWUVVTKqSw = false;}
      if(SZbfClUdYL == true){SZbfClUdYL = false;}
      if(MdChxwGFfG == true){MdChxwGFfG = false;}
      if(JIFhQHojBb == true){JIFhQHojBb = false;}
      if(czoXrKkhWM == true){czoXrKkhWM = false;}
      if(HcYlBcUmXZ == true){HcYlBcUmXZ = false;}
      if(NFKLlJUtPt == true){NFKLlJUtPt = false;}
      if(wkbxhNndpm == true){wkbxhNndpm = false;}
      if(KXhSygjSfm == true){KXhSygjSfm = false;}
      if(mBLaNXnckh == true){mBLaNXnckh = false;}
      if(opSbQWAbDn == true){opSbQWAbDn = false;}
      if(opxkOpWrNF == true){opxkOpWrNF = false;}
      if(YCNbHtYAiK == true){YCNbHtYAiK = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LMJUSPNWSG
{ 
  void PEEGMHWJdi()
  { 
      bool swjdoGARPA = false;
      bool HNkUIEdjdJ = false;
      bool qgSERFuOfz = false;
      bool FIdnWteJeu = false;
      bool tprXtXMpbF = false;
      bool rrLBiMceLj = false;
      bool KlkQuzjwxN = false;
      bool YUFiPBnuKt = false;
      bool rFlZHMxbEK = false;
      bool RAxujHEbbu = false;
      bool nQsOgXlblh = false;
      bool XgURqFzLIh = false;
      bool BHqmwYXJyH = false;
      bool wMTOFbMXsj = false;
      bool aUpWljeeEe = false;
      bool HsSssgZFSH = false;
      bool zccdlKNGLO = false;
      bool sbSYHgQUAL = false;
      bool trFTzCtaUY = false;
      bool CGFljxqLja = false;
      string PAYoHIZKdp;
      string xYBmPHWIyh;
      string JDXUpSQnLA;
      string GfokEupGNn;
      string CgIfPIWZFw;
      string rfdZZLbDyn;
      string rlydjRiPiT;
      string RPtKebuqFh;
      string uphOHZBPLr;
      string TlIZuQidwy;
      string YGqleblTtx;
      string QjyPrCDiVJ;
      string PTEqAeHUOO;
      string sQuGInywje;
      string nhUOainrgs;
      string MzIlhFCAkz;
      string URFfDAoqHD;
      string DGzpVhyoPQ;
      string qPmUBtIIXC;
      string kyKkKWboKa;
      if(PAYoHIZKdp == YGqleblTtx){swjdoGARPA = true;}
      else if(YGqleblTtx == PAYoHIZKdp){nQsOgXlblh = true;}
      if(xYBmPHWIyh == QjyPrCDiVJ){HNkUIEdjdJ = true;}
      else if(QjyPrCDiVJ == xYBmPHWIyh){XgURqFzLIh = true;}
      if(JDXUpSQnLA == PTEqAeHUOO){qgSERFuOfz = true;}
      else if(PTEqAeHUOO == JDXUpSQnLA){BHqmwYXJyH = true;}
      if(GfokEupGNn == sQuGInywje){FIdnWteJeu = true;}
      else if(sQuGInywje == GfokEupGNn){wMTOFbMXsj = true;}
      if(CgIfPIWZFw == nhUOainrgs){tprXtXMpbF = true;}
      else if(nhUOainrgs == CgIfPIWZFw){aUpWljeeEe = true;}
      if(rfdZZLbDyn == MzIlhFCAkz){rrLBiMceLj = true;}
      else if(MzIlhFCAkz == rfdZZLbDyn){HsSssgZFSH = true;}
      if(rlydjRiPiT == URFfDAoqHD){KlkQuzjwxN = true;}
      else if(URFfDAoqHD == rlydjRiPiT){zccdlKNGLO = true;}
      if(RPtKebuqFh == DGzpVhyoPQ){YUFiPBnuKt = true;}
      if(uphOHZBPLr == qPmUBtIIXC){rFlZHMxbEK = true;}
      if(TlIZuQidwy == kyKkKWboKa){RAxujHEbbu = true;}
      while(DGzpVhyoPQ == RPtKebuqFh){sbSYHgQUAL = true;}
      while(qPmUBtIIXC == qPmUBtIIXC){trFTzCtaUY = true;}
      while(kyKkKWboKa == kyKkKWboKa){CGFljxqLja = true;}
      if(swjdoGARPA == true){swjdoGARPA = false;}
      if(HNkUIEdjdJ == true){HNkUIEdjdJ = false;}
      if(qgSERFuOfz == true){qgSERFuOfz = false;}
      if(FIdnWteJeu == true){FIdnWteJeu = false;}
      if(tprXtXMpbF == true){tprXtXMpbF = false;}
      if(rrLBiMceLj == true){rrLBiMceLj = false;}
      if(KlkQuzjwxN == true){KlkQuzjwxN = false;}
      if(YUFiPBnuKt == true){YUFiPBnuKt = false;}
      if(rFlZHMxbEK == true){rFlZHMxbEK = false;}
      if(RAxujHEbbu == true){RAxujHEbbu = false;}
      if(nQsOgXlblh == true){nQsOgXlblh = false;}
      if(XgURqFzLIh == true){XgURqFzLIh = false;}
      if(BHqmwYXJyH == true){BHqmwYXJyH = false;}
      if(wMTOFbMXsj == true){wMTOFbMXsj = false;}
      if(aUpWljeeEe == true){aUpWljeeEe = false;}
      if(HsSssgZFSH == true){HsSssgZFSH = false;}
      if(zccdlKNGLO == true){zccdlKNGLO = false;}
      if(sbSYHgQUAL == true){sbSYHgQUAL = false;}
      if(trFTzCtaUY == true){trFTzCtaUY = false;}
      if(CGFljxqLja == true){CGFljxqLja = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JCEPODUICL
{ 
  void LVUufiOdHo()
  { 
      bool JYMFWMkXRB = false;
      bool tAnIzWuJHV = false;
      bool KfgkzfNMph = false;
      bool fewMYleimF = false;
      bool gJoOEoNZbw = false;
      bool hQgkAdrbFq = false;
      bool zXgiUPEaiN = false;
      bool rMWCnBpSfJ = false;
      bool RmksAPxKLt = false;
      bool sCcwFIbFsn = false;
      bool klqOEUsdwz = false;
      bool nZuuYLlniF = false;
      bool RsOnToiaxf = false;
      bool YkYrYfwVoe = false;
      bool VTNMGTHgWs = false;
      bool VnVnVqXCuO = false;
      bool lSgQZCADWs = false;
      bool PQyEiyeCgz = false;
      bool hCDDJkOkVg = false;
      bool HsaasDlxlE = false;
      string ftURAZhzxT;
      string BVUkWPjbLA;
      string wnnTJUbQtU;
      string PXVxBYrCLx;
      string uEBEmxYYUP;
      string jjuFWrGBIf;
      string yREbDdzXjx;
      string XhPNErmjgL;
      string uCwdjfbQZI;
      string SpsfzdAYAJ;
      string yBnWrCThAe;
      string FVUxjuUaqX;
      string BixbegnjNa;
      string qNiXNtNZlh;
      string MtPtDwVwuE;
      string IZgcHODMqK;
      string OiTZmqlwjp;
      string gunJKGTDYi;
      string LqnJRRgGPy;
      string pBhaTcaoGL;
      if(ftURAZhzxT == yBnWrCThAe){JYMFWMkXRB = true;}
      else if(yBnWrCThAe == ftURAZhzxT){klqOEUsdwz = true;}
      if(BVUkWPjbLA == FVUxjuUaqX){tAnIzWuJHV = true;}
      else if(FVUxjuUaqX == BVUkWPjbLA){nZuuYLlniF = true;}
      if(wnnTJUbQtU == BixbegnjNa){KfgkzfNMph = true;}
      else if(BixbegnjNa == wnnTJUbQtU){RsOnToiaxf = true;}
      if(PXVxBYrCLx == qNiXNtNZlh){fewMYleimF = true;}
      else if(qNiXNtNZlh == PXVxBYrCLx){YkYrYfwVoe = true;}
      if(uEBEmxYYUP == MtPtDwVwuE){gJoOEoNZbw = true;}
      else if(MtPtDwVwuE == uEBEmxYYUP){VTNMGTHgWs = true;}
      if(jjuFWrGBIf == IZgcHODMqK){hQgkAdrbFq = true;}
      else if(IZgcHODMqK == jjuFWrGBIf){VnVnVqXCuO = true;}
      if(yREbDdzXjx == OiTZmqlwjp){zXgiUPEaiN = true;}
      else if(OiTZmqlwjp == yREbDdzXjx){lSgQZCADWs = true;}
      if(XhPNErmjgL == gunJKGTDYi){rMWCnBpSfJ = true;}
      if(uCwdjfbQZI == LqnJRRgGPy){RmksAPxKLt = true;}
      if(SpsfzdAYAJ == pBhaTcaoGL){sCcwFIbFsn = true;}
      while(gunJKGTDYi == XhPNErmjgL){PQyEiyeCgz = true;}
      while(LqnJRRgGPy == LqnJRRgGPy){hCDDJkOkVg = true;}
      while(pBhaTcaoGL == pBhaTcaoGL){HsaasDlxlE = true;}
      if(JYMFWMkXRB == true){JYMFWMkXRB = false;}
      if(tAnIzWuJHV == true){tAnIzWuJHV = false;}
      if(KfgkzfNMph == true){KfgkzfNMph = false;}
      if(fewMYleimF == true){fewMYleimF = false;}
      if(gJoOEoNZbw == true){gJoOEoNZbw = false;}
      if(hQgkAdrbFq == true){hQgkAdrbFq = false;}
      if(zXgiUPEaiN == true){zXgiUPEaiN = false;}
      if(rMWCnBpSfJ == true){rMWCnBpSfJ = false;}
      if(RmksAPxKLt == true){RmksAPxKLt = false;}
      if(sCcwFIbFsn == true){sCcwFIbFsn = false;}
      if(klqOEUsdwz == true){klqOEUsdwz = false;}
      if(nZuuYLlniF == true){nZuuYLlniF = false;}
      if(RsOnToiaxf == true){RsOnToiaxf = false;}
      if(YkYrYfwVoe == true){YkYrYfwVoe = false;}
      if(VTNMGTHgWs == true){VTNMGTHgWs = false;}
      if(VnVnVqXCuO == true){VnVnVqXCuO = false;}
      if(lSgQZCADWs == true){lSgQZCADWs = false;}
      if(PQyEiyeCgz == true){PQyEiyeCgz = false;}
      if(hCDDJkOkVg == true){hCDDJkOkVg = false;}
      if(HsaasDlxlE == true){HsaasDlxlE = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PFFOSCKVCK
{ 
  void rrjgMUadhU()
  { 
      bool FFZmTdzAsd = false;
      bool fUCUxekjYM = false;
      bool GqCMIuBGaI = false;
      bool XhLhKScrBP = false;
      bool uygWdhodCw = false;
      bool NNrQLNDMdY = false;
      bool bsYkFyIUKe = false;
      bool BsiJQHKeia = false;
      bool GXDetKouWw = false;
      bool OQNbnCZwQf = false;
      bool YUSXuUdizE = false;
      bool CYdCSEotpu = false;
      bool INxOPkeGUO = false;
      bool hONDCkEfjZ = false;
      bool fGuSsplLJj = false;
      bool MfFUWLWEHK = false;
      bool oXOjWnKaAg = false;
      bool zIKPPCSeUZ = false;
      bool yTwkCBIsfq = false;
      bool OzYYkHqKwF = false;
      string INfNNKZVos;
      string xeCHQgqnRB;
      string OjPFquDgVk;
      string HmOiDNmgnx;
      string KgSMghBfZx;
      string suCjpYxaVR;
      string GszEkJQSDC;
      string tKATphQDXI;
      string tXHYrMkdse;
      string QMKVqQgOaX;
      string RibWDhuhsI;
      string HsDNWilaLY;
      string XlYXlgAdcY;
      string ABbApLujUJ;
      string QCkjXWIwyH;
      string eFCBzfJDkV;
      string hQFTHFRDWL;
      string mBFCIUAoAP;
      string YDeUtuzZAJ;
      string CrzzazxRcR;
      if(INfNNKZVos == RibWDhuhsI){FFZmTdzAsd = true;}
      else if(RibWDhuhsI == INfNNKZVos){YUSXuUdizE = true;}
      if(xeCHQgqnRB == HsDNWilaLY){fUCUxekjYM = true;}
      else if(HsDNWilaLY == xeCHQgqnRB){CYdCSEotpu = true;}
      if(OjPFquDgVk == XlYXlgAdcY){GqCMIuBGaI = true;}
      else if(XlYXlgAdcY == OjPFquDgVk){INxOPkeGUO = true;}
      if(HmOiDNmgnx == ABbApLujUJ){XhLhKScrBP = true;}
      else if(ABbApLujUJ == HmOiDNmgnx){hONDCkEfjZ = true;}
      if(KgSMghBfZx == QCkjXWIwyH){uygWdhodCw = true;}
      else if(QCkjXWIwyH == KgSMghBfZx){fGuSsplLJj = true;}
      if(suCjpYxaVR == eFCBzfJDkV){NNrQLNDMdY = true;}
      else if(eFCBzfJDkV == suCjpYxaVR){MfFUWLWEHK = true;}
      if(GszEkJQSDC == hQFTHFRDWL){bsYkFyIUKe = true;}
      else if(hQFTHFRDWL == GszEkJQSDC){oXOjWnKaAg = true;}
      if(tKATphQDXI == mBFCIUAoAP){BsiJQHKeia = true;}
      if(tXHYrMkdse == YDeUtuzZAJ){GXDetKouWw = true;}
      if(QMKVqQgOaX == CrzzazxRcR){OQNbnCZwQf = true;}
      while(mBFCIUAoAP == tKATphQDXI){zIKPPCSeUZ = true;}
      while(YDeUtuzZAJ == YDeUtuzZAJ){yTwkCBIsfq = true;}
      while(CrzzazxRcR == CrzzazxRcR){OzYYkHqKwF = true;}
      if(FFZmTdzAsd == true){FFZmTdzAsd = false;}
      if(fUCUxekjYM == true){fUCUxekjYM = false;}
      if(GqCMIuBGaI == true){GqCMIuBGaI = false;}
      if(XhLhKScrBP == true){XhLhKScrBP = false;}
      if(uygWdhodCw == true){uygWdhodCw = false;}
      if(NNrQLNDMdY == true){NNrQLNDMdY = false;}
      if(bsYkFyIUKe == true){bsYkFyIUKe = false;}
      if(BsiJQHKeia == true){BsiJQHKeia = false;}
      if(GXDetKouWw == true){GXDetKouWw = false;}
      if(OQNbnCZwQf == true){OQNbnCZwQf = false;}
      if(YUSXuUdizE == true){YUSXuUdizE = false;}
      if(CYdCSEotpu == true){CYdCSEotpu = false;}
      if(INxOPkeGUO == true){INxOPkeGUO = false;}
      if(hONDCkEfjZ == true){hONDCkEfjZ = false;}
      if(fGuSsplLJj == true){fGuSsplLJj = false;}
      if(MfFUWLWEHK == true){MfFUWLWEHK = false;}
      if(oXOjWnKaAg == true){oXOjWnKaAg = false;}
      if(zIKPPCSeUZ == true){zIKPPCSeUZ = false;}
      if(yTwkCBIsfq == true){yTwkCBIsfq = false;}
      if(OzYYkHqKwF == true){OzYYkHqKwF = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class HNGGIVKYKH
{ 
  void blBSPJEfPn()
  { 
      bool CsyzkDcZPR = false;
      bool MLQOpefFoq = false;
      bool lkBJjLHCMk = false;
      bool DteRfWTRPF = false;
      bool gmqmezCaLU = false;
      bool TTUHeQqnOZ = false;
      bool wZMdOSfyhT = false;
      bool LNoiJztOdk = false;
      bool IfqrQiZXlP = false;
      bool feDpJGZMYM = false;
      bool SEsqIFdkxf = false;
      bool WcKdQPhUNe = false;
      bool sILRngKzZC = false;
      bool YgoRRElziK = false;
      bool FnZGrCUYYs = false;
      bool YFztSKraDb = false;
      bool ueLtHomrLM = false;
      bool chkCZEhFZk = false;
      bool MkZfdShuyH = false;
      bool MLwGPZbfyD = false;
      string PKnnjFOWnY;
      string XuxZIwtXOf;
      string bUUZpOlOcN;
      string eJTZJjWMOh;
      string koXdTDrQAx;
      string zTEDgupNyU;
      string TzsZtOaXPX;
      string ZhIfumwqFl;
      string WUOfOKEPsO;
      string tINtOkuxjF;
      string bUBbXQLXIs;
      string yzIMyyjNKi;
      string JLdAdQTNQJ;
      string rzGAJBTmIx;
      string yfHAQuABwP;
      string AhlAuQpAxG;
      string xyGyByrHCw;
      string exEMkcDdji;
      string PRRBJJMGFr;
      string LSoBAAZJyR;
      if(PKnnjFOWnY == bUBbXQLXIs){CsyzkDcZPR = true;}
      else if(bUBbXQLXIs == PKnnjFOWnY){SEsqIFdkxf = true;}
      if(XuxZIwtXOf == yzIMyyjNKi){MLQOpefFoq = true;}
      else if(yzIMyyjNKi == XuxZIwtXOf){WcKdQPhUNe = true;}
      if(bUUZpOlOcN == JLdAdQTNQJ){lkBJjLHCMk = true;}
      else if(JLdAdQTNQJ == bUUZpOlOcN){sILRngKzZC = true;}
      if(eJTZJjWMOh == rzGAJBTmIx){DteRfWTRPF = true;}
      else if(rzGAJBTmIx == eJTZJjWMOh){YgoRRElziK = true;}
      if(koXdTDrQAx == yfHAQuABwP){gmqmezCaLU = true;}
      else if(yfHAQuABwP == koXdTDrQAx){FnZGrCUYYs = true;}
      if(zTEDgupNyU == AhlAuQpAxG){TTUHeQqnOZ = true;}
      else if(AhlAuQpAxG == zTEDgupNyU){YFztSKraDb = true;}
      if(TzsZtOaXPX == xyGyByrHCw){wZMdOSfyhT = true;}
      else if(xyGyByrHCw == TzsZtOaXPX){ueLtHomrLM = true;}
      if(ZhIfumwqFl == exEMkcDdji){LNoiJztOdk = true;}
      if(WUOfOKEPsO == PRRBJJMGFr){IfqrQiZXlP = true;}
      if(tINtOkuxjF == LSoBAAZJyR){feDpJGZMYM = true;}
      while(exEMkcDdji == ZhIfumwqFl){chkCZEhFZk = true;}
      while(PRRBJJMGFr == PRRBJJMGFr){MkZfdShuyH = true;}
      while(LSoBAAZJyR == LSoBAAZJyR){MLwGPZbfyD = true;}
      if(CsyzkDcZPR == true){CsyzkDcZPR = false;}
      if(MLQOpefFoq == true){MLQOpefFoq = false;}
      if(lkBJjLHCMk == true){lkBJjLHCMk = false;}
      if(DteRfWTRPF == true){DteRfWTRPF = false;}
      if(gmqmezCaLU == true){gmqmezCaLU = false;}
      if(TTUHeQqnOZ == true){TTUHeQqnOZ = false;}
      if(wZMdOSfyhT == true){wZMdOSfyhT = false;}
      if(LNoiJztOdk == true){LNoiJztOdk = false;}
      if(IfqrQiZXlP == true){IfqrQiZXlP = false;}
      if(feDpJGZMYM == true){feDpJGZMYM = false;}
      if(SEsqIFdkxf == true){SEsqIFdkxf = false;}
      if(WcKdQPhUNe == true){WcKdQPhUNe = false;}
      if(sILRngKzZC == true){sILRngKzZC = false;}
      if(YgoRRElziK == true){YgoRRElziK = false;}
      if(FnZGrCUYYs == true){FnZGrCUYYs = false;}
      if(YFztSKraDb == true){YFztSKraDb = false;}
      if(ueLtHomrLM == true){ueLtHomrLM = false;}
      if(chkCZEhFZk == true){chkCZEhFZk = false;}
      if(MkZfdShuyH == true){MkZfdShuyH = false;}
      if(MLwGPZbfyD == true){MLwGPZbfyD = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class DIXTARVJYE
{ 
  void ROqsmaMldV()
  { 
      bool tBKPqnyqjq = false;
      bool esobNuKiQZ = false;
      bool JMMlAgFIie = false;
      bool VgJUxIfibF = false;
      bool RXnyaHfzXT = false;
      bool SoheahnXPJ = false;
      bool ipHEUxjUeE = false;
      bool OaGLIlXOLs = false;
      bool AFkPMfzdsO = false;
      bool UITRWaPicc = false;
      bool WLUTErhWWo = false;
      bool KZMWJcFrro = false;
      bool yGaQjGHong = false;
      bool VnlhVnjRKT = false;
      bool xcGEDtuwIs = false;
      bool VXqmeQkEiy = false;
      bool kshoTVneBU = false;
      bool rnhXqVzWMo = false;
      bool LKguKxxyDE = false;
      bool DfhLiEtRjh = false;
      string qaDalbCWCn;
      string psEGDhzxja;
      string xTMuuBhlZT;
      string cgTyHwnrII;
      string OsYDOaZHye;
      string mOqlDWNIej;
      string tQxhLJYGyG;
      string sGnbMEuJfU;
      string bZmtgNohuj;
      string ifNYOyupqF;
      string CTIqkfZPYz;
      string bDbKITcCAx;
      string TpLNXNsPNO;
      string IESaRIQuIB;
      string nZJwIfDCPt;
      string UrpskLHuTX;
      string lKBKpXkBrA;
      string CgszJtgOFr;
      string KpzSsPOYoG;
      string AVbkPdXzFj;
      if(qaDalbCWCn == CTIqkfZPYz){tBKPqnyqjq = true;}
      else if(CTIqkfZPYz == qaDalbCWCn){WLUTErhWWo = true;}
      if(psEGDhzxja == bDbKITcCAx){esobNuKiQZ = true;}
      else if(bDbKITcCAx == psEGDhzxja){KZMWJcFrro = true;}
      if(xTMuuBhlZT == TpLNXNsPNO){JMMlAgFIie = true;}
      else if(TpLNXNsPNO == xTMuuBhlZT){yGaQjGHong = true;}
      if(cgTyHwnrII == IESaRIQuIB){VgJUxIfibF = true;}
      else if(IESaRIQuIB == cgTyHwnrII){VnlhVnjRKT = true;}
      if(OsYDOaZHye == nZJwIfDCPt){RXnyaHfzXT = true;}
      else if(nZJwIfDCPt == OsYDOaZHye){xcGEDtuwIs = true;}
      if(mOqlDWNIej == UrpskLHuTX){SoheahnXPJ = true;}
      else if(UrpskLHuTX == mOqlDWNIej){VXqmeQkEiy = true;}
      if(tQxhLJYGyG == lKBKpXkBrA){ipHEUxjUeE = true;}
      else if(lKBKpXkBrA == tQxhLJYGyG){kshoTVneBU = true;}
      if(sGnbMEuJfU == CgszJtgOFr){OaGLIlXOLs = true;}
      if(bZmtgNohuj == KpzSsPOYoG){AFkPMfzdsO = true;}
      if(ifNYOyupqF == AVbkPdXzFj){UITRWaPicc = true;}
      while(CgszJtgOFr == sGnbMEuJfU){rnhXqVzWMo = true;}
      while(KpzSsPOYoG == KpzSsPOYoG){LKguKxxyDE = true;}
      while(AVbkPdXzFj == AVbkPdXzFj){DfhLiEtRjh = true;}
      if(tBKPqnyqjq == true){tBKPqnyqjq = false;}
      if(esobNuKiQZ == true){esobNuKiQZ = false;}
      if(JMMlAgFIie == true){JMMlAgFIie = false;}
      if(VgJUxIfibF == true){VgJUxIfibF = false;}
      if(RXnyaHfzXT == true){RXnyaHfzXT = false;}
      if(SoheahnXPJ == true){SoheahnXPJ = false;}
      if(ipHEUxjUeE == true){ipHEUxjUeE = false;}
      if(OaGLIlXOLs == true){OaGLIlXOLs = false;}
      if(AFkPMfzdsO == true){AFkPMfzdsO = false;}
      if(UITRWaPicc == true){UITRWaPicc = false;}
      if(WLUTErhWWo == true){WLUTErhWWo = false;}
      if(KZMWJcFrro == true){KZMWJcFrro = false;}
      if(yGaQjGHong == true){yGaQjGHong = false;}
      if(VnlhVnjRKT == true){VnlhVnjRKT = false;}
      if(xcGEDtuwIs == true){xcGEDtuwIs = false;}
      if(VXqmeQkEiy == true){VXqmeQkEiy = false;}
      if(kshoTVneBU == true){kshoTVneBU = false;}
      if(rnhXqVzWMo == true){rnhXqVzWMo = false;}
      if(LKguKxxyDE == true){LKguKxxyDE = false;}
      if(DfhLiEtRjh == true){DfhLiEtRjh = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class EMLEHNJOBE
{ 
  void qjkiAharQC()
  { 
      bool PZXlTBimrk = false;
      bool WpqpMVASCS = false;
      bool SQwOwexASi = false;
      bool VKZgzNPUDT = false;
      bool cPEfeIGsnZ = false;
      bool hpzVxydIon = false;
      bool EiUJOMPJqz = false;
      bool GeOioCMQim = false;
      bool XDMsBfKDiA = false;
      bool jGfOPtutRR = false;
      bool rwCoTCbQgu = false;
      bool kMYBQAVbHi = false;
      bool BMZpOAzzSt = false;
      bool GRzRuaBXZy = false;
      bool DtWnOXgdyO = false;
      bool YTmhQfVLZS = false;
      bool DbxlnTNHGp = false;
      bool zxBJeGolaq = false;
      bool SUBEVleQAO = false;
      bool EoyGubxkzu = false;
      string KQheqOhMyr;
      string kWtUeVVlUP;
      string RFaJSElaZp;
      string ByiPUmICRn;
      string JccGhKQZor;
      string CQogSEoXLl;
      string bGlHqmneAq;
      string NSTfGnbUbI;
      string nMlhnkIBxP;
      string yjqeLCpVOt;
      string WkYxejwJad;
      string cTuCyPUESF;
      string dlgjscanTD;
      string yGFglCmnCE;
      string ozZTmdnngw;
      string hkCJPjDoEB;
      string IHpXeWNNQu;
      string cfBOQeMNxO;
      string nmHWUMDwQC;
      string yyiAuhIBnQ;
      if(KQheqOhMyr == WkYxejwJad){PZXlTBimrk = true;}
      else if(WkYxejwJad == KQheqOhMyr){rwCoTCbQgu = true;}
      if(kWtUeVVlUP == cTuCyPUESF){WpqpMVASCS = true;}
      else if(cTuCyPUESF == kWtUeVVlUP){kMYBQAVbHi = true;}
      if(RFaJSElaZp == dlgjscanTD){SQwOwexASi = true;}
      else if(dlgjscanTD == RFaJSElaZp){BMZpOAzzSt = true;}
      if(ByiPUmICRn == yGFglCmnCE){VKZgzNPUDT = true;}
      else if(yGFglCmnCE == ByiPUmICRn){GRzRuaBXZy = true;}
      if(JccGhKQZor == ozZTmdnngw){cPEfeIGsnZ = true;}
      else if(ozZTmdnngw == JccGhKQZor){DtWnOXgdyO = true;}
      if(CQogSEoXLl == hkCJPjDoEB){hpzVxydIon = true;}
      else if(hkCJPjDoEB == CQogSEoXLl){YTmhQfVLZS = true;}
      if(bGlHqmneAq == IHpXeWNNQu){EiUJOMPJqz = true;}
      else if(IHpXeWNNQu == bGlHqmneAq){DbxlnTNHGp = true;}
      if(NSTfGnbUbI == cfBOQeMNxO){GeOioCMQim = true;}
      if(nMlhnkIBxP == nmHWUMDwQC){XDMsBfKDiA = true;}
      if(yjqeLCpVOt == yyiAuhIBnQ){jGfOPtutRR = true;}
      while(cfBOQeMNxO == NSTfGnbUbI){zxBJeGolaq = true;}
      while(nmHWUMDwQC == nmHWUMDwQC){SUBEVleQAO = true;}
      while(yyiAuhIBnQ == yyiAuhIBnQ){EoyGubxkzu = true;}
      if(PZXlTBimrk == true){PZXlTBimrk = false;}
      if(WpqpMVASCS == true){WpqpMVASCS = false;}
      if(SQwOwexASi == true){SQwOwexASi = false;}
      if(VKZgzNPUDT == true){VKZgzNPUDT = false;}
      if(cPEfeIGsnZ == true){cPEfeIGsnZ = false;}
      if(hpzVxydIon == true){hpzVxydIon = false;}
      if(EiUJOMPJqz == true){EiUJOMPJqz = false;}
      if(GeOioCMQim == true){GeOioCMQim = false;}
      if(XDMsBfKDiA == true){XDMsBfKDiA = false;}
      if(jGfOPtutRR == true){jGfOPtutRR = false;}
      if(rwCoTCbQgu == true){rwCoTCbQgu = false;}
      if(kMYBQAVbHi == true){kMYBQAVbHi = false;}
      if(BMZpOAzzSt == true){BMZpOAzzSt = false;}
      if(GRzRuaBXZy == true){GRzRuaBXZy = false;}
      if(DtWnOXgdyO == true){DtWnOXgdyO = false;}
      if(YTmhQfVLZS == true){YTmhQfVLZS = false;}
      if(DbxlnTNHGp == true){DbxlnTNHGp = false;}
      if(zxBJeGolaq == true){zxBJeGolaq = false;}
      if(SUBEVleQAO == true){SUBEVleQAO = false;}
      if(EoyGubxkzu == true){EoyGubxkzu = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SYUJFXTTDS
{ 
  void FpJphYMYqp()
  { 
      bool IZgBHQakiy = false;
      bool TtEomQuMOV = false;
      bool AXUOJWCuDT = false;
      bool nJPLSxpEVb = false;
      bool FJPqkYnBLb = false;
      bool VcIJljFSqk = false;
      bool OfgPJKerWp = false;
      bool ZqplPpIdcj = false;
      bool iBiayKAFZi = false;
      bool rXMkNZbzga = false;
      bool swdVCdlVtu = false;
      bool baoGBMwGQz = false;
      bool JZRnxBeEYp = false;
      bool NIwCplWNYU = false;
      bool XdLdGQiFiA = false;
      bool cVigQTdpXA = false;
      bool yhxDhTelPP = false;
      bool pihwaeOngg = false;
      bool SZUXMrYOlW = false;
      bool IfpXVpgHwl = false;
      string jBCWhmJyaA;
      string qTpCYIMnKH;
      string hXVYgQxcQC;
      string sGQWQzoOjY;
      string rGieWPmTxn;
      string JoeZhLfcpR;
      string oggXHkCSkZ;
      string TxDlObnpCR;
      string ddecsDapoF;
      string TLYSiLYLXl;
      string MThWTMksLG;
      string XoaYnLBDdD;
      string KLdfGIXjCD;
      string CbSQFJuHJg;
      string RgeigejAeX;
      string KUgTaApCQK;
      string nBqHDuVKSd;
      string Psswkcguya;
      string XXFdsguyoE;
      string TrpysQoXel;
      if(jBCWhmJyaA == MThWTMksLG){IZgBHQakiy = true;}
      else if(MThWTMksLG == jBCWhmJyaA){swdVCdlVtu = true;}
      if(qTpCYIMnKH == XoaYnLBDdD){TtEomQuMOV = true;}
      else if(XoaYnLBDdD == qTpCYIMnKH){baoGBMwGQz = true;}
      if(hXVYgQxcQC == KLdfGIXjCD){AXUOJWCuDT = true;}
      else if(KLdfGIXjCD == hXVYgQxcQC){JZRnxBeEYp = true;}
      if(sGQWQzoOjY == CbSQFJuHJg){nJPLSxpEVb = true;}
      else if(CbSQFJuHJg == sGQWQzoOjY){NIwCplWNYU = true;}
      if(rGieWPmTxn == RgeigejAeX){FJPqkYnBLb = true;}
      else if(RgeigejAeX == rGieWPmTxn){XdLdGQiFiA = true;}
      if(JoeZhLfcpR == KUgTaApCQK){VcIJljFSqk = true;}
      else if(KUgTaApCQK == JoeZhLfcpR){cVigQTdpXA = true;}
      if(oggXHkCSkZ == nBqHDuVKSd){OfgPJKerWp = true;}
      else if(nBqHDuVKSd == oggXHkCSkZ){yhxDhTelPP = true;}
      if(TxDlObnpCR == Psswkcguya){ZqplPpIdcj = true;}
      if(ddecsDapoF == XXFdsguyoE){iBiayKAFZi = true;}
      if(TLYSiLYLXl == TrpysQoXel){rXMkNZbzga = true;}
      while(Psswkcguya == TxDlObnpCR){pihwaeOngg = true;}
      while(XXFdsguyoE == XXFdsguyoE){SZUXMrYOlW = true;}
      while(TrpysQoXel == TrpysQoXel){IfpXVpgHwl = true;}
      if(IZgBHQakiy == true){IZgBHQakiy = false;}
      if(TtEomQuMOV == true){TtEomQuMOV = false;}
      if(AXUOJWCuDT == true){AXUOJWCuDT = false;}
      if(nJPLSxpEVb == true){nJPLSxpEVb = false;}
      if(FJPqkYnBLb == true){FJPqkYnBLb = false;}
      if(VcIJljFSqk == true){VcIJljFSqk = false;}
      if(OfgPJKerWp == true){OfgPJKerWp = false;}
      if(ZqplPpIdcj == true){ZqplPpIdcj = false;}
      if(iBiayKAFZi == true){iBiayKAFZi = false;}
      if(rXMkNZbzga == true){rXMkNZbzga = false;}
      if(swdVCdlVtu == true){swdVCdlVtu = false;}
      if(baoGBMwGQz == true){baoGBMwGQz = false;}
      if(JZRnxBeEYp == true){JZRnxBeEYp = false;}
      if(NIwCplWNYU == true){NIwCplWNYU = false;}
      if(XdLdGQiFiA == true){XdLdGQiFiA = false;}
      if(cVigQTdpXA == true){cVigQTdpXA = false;}
      if(yhxDhTelPP == true){yhxDhTelPP = false;}
      if(pihwaeOngg == true){pihwaeOngg = false;}
      if(SZUXMrYOlW == true){SZUXMrYOlW = false;}
      if(IfpXVpgHwl == true){IfpXVpgHwl = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZZURSBKWMT
{ 
  void fljbySEfAX()
  { 
      bool dcTPLIhEAY = false;
      bool OgVbixcdXl = false;
      bool ezAEWLWmWi = false;
      bool RRypsoqrme = false;
      bool sqNeoSqicl = false;
      bool NsbTGebOsA = false;
      bool kugRHwhfht = false;
      bool GjpsrmkVPD = false;
      bool TnKjUXpdjO = false;
      bool NbzmsyJwXs = false;
      bool jLBhfcDibS = false;
      bool sGzofxkwwi = false;
      bool GDuSgdjDzt = false;
      bool doyVTIlbEq = false;
      bool THCQUGbKHw = false;
      bool MMznlgPhBB = false;
      bool sAigoDIcVW = false;
      bool KJrFPyAdxZ = false;
      bool CZuDgQpJjl = false;
      bool rEZCyTXUbR = false;
      string NNPgYpVEod;
      string lliXSwfeGR;
      string fPxZnWLBYB;
      string VhFSrJrRtA;
      string DayhGqaQZG;
      string LPTDbyQorC;
      string eTfnHDfPfJ;
      string DOkGNEQOTx;
      string utaRoGNCqs;
      string sDYqViuJaC;
      string LKjQCxdNpW;
      string gBVuGjsNyB;
      string odYmQAoskd;
      string JGnQlkKKOr;
      string bALTLrhdkk;
      string pgKZNglxAC;
      string IkcLNaNEJV;
      string YhzGYoREaX;
      string SKIzmbuNDE;
      string zNKMjpXAGk;
      if(NNPgYpVEod == LKjQCxdNpW){dcTPLIhEAY = true;}
      else if(LKjQCxdNpW == NNPgYpVEod){jLBhfcDibS = true;}
      if(lliXSwfeGR == gBVuGjsNyB){OgVbixcdXl = true;}
      else if(gBVuGjsNyB == lliXSwfeGR){sGzofxkwwi = true;}
      if(fPxZnWLBYB == odYmQAoskd){ezAEWLWmWi = true;}
      else if(odYmQAoskd == fPxZnWLBYB){GDuSgdjDzt = true;}
      if(VhFSrJrRtA == JGnQlkKKOr){RRypsoqrme = true;}
      else if(JGnQlkKKOr == VhFSrJrRtA){doyVTIlbEq = true;}
      if(DayhGqaQZG == bALTLrhdkk){sqNeoSqicl = true;}
      else if(bALTLrhdkk == DayhGqaQZG){THCQUGbKHw = true;}
      if(LPTDbyQorC == pgKZNglxAC){NsbTGebOsA = true;}
      else if(pgKZNglxAC == LPTDbyQorC){MMznlgPhBB = true;}
      if(eTfnHDfPfJ == IkcLNaNEJV){kugRHwhfht = true;}
      else if(IkcLNaNEJV == eTfnHDfPfJ){sAigoDIcVW = true;}
      if(DOkGNEQOTx == YhzGYoREaX){GjpsrmkVPD = true;}
      if(utaRoGNCqs == SKIzmbuNDE){TnKjUXpdjO = true;}
      if(sDYqViuJaC == zNKMjpXAGk){NbzmsyJwXs = true;}
      while(YhzGYoREaX == DOkGNEQOTx){KJrFPyAdxZ = true;}
      while(SKIzmbuNDE == SKIzmbuNDE){CZuDgQpJjl = true;}
      while(zNKMjpXAGk == zNKMjpXAGk){rEZCyTXUbR = true;}
      if(dcTPLIhEAY == true){dcTPLIhEAY = false;}
      if(OgVbixcdXl == true){OgVbixcdXl = false;}
      if(ezAEWLWmWi == true){ezAEWLWmWi = false;}
      if(RRypsoqrme == true){RRypsoqrme = false;}
      if(sqNeoSqicl == true){sqNeoSqicl = false;}
      if(NsbTGebOsA == true){NsbTGebOsA = false;}
      if(kugRHwhfht == true){kugRHwhfht = false;}
      if(GjpsrmkVPD == true){GjpsrmkVPD = false;}
      if(TnKjUXpdjO == true){TnKjUXpdjO = false;}
      if(NbzmsyJwXs == true){NbzmsyJwXs = false;}
      if(jLBhfcDibS == true){jLBhfcDibS = false;}
      if(sGzofxkwwi == true){sGzofxkwwi = false;}
      if(GDuSgdjDzt == true){GDuSgdjDzt = false;}
      if(doyVTIlbEq == true){doyVTIlbEq = false;}
      if(THCQUGbKHw == true){THCQUGbKHw = false;}
      if(MMznlgPhBB == true){MMznlgPhBB = false;}
      if(sAigoDIcVW == true){sAigoDIcVW = false;}
      if(KJrFPyAdxZ == true){KJrFPyAdxZ = false;}
      if(CZuDgQpJjl == true){CZuDgQpJjl = false;}
      if(rEZCyTXUbR == true){rEZCyTXUbR = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LCTXUAICYK
{ 
  void rYynBzwpuq()
  { 
      bool IKzfhQLJUi = false;
      bool kXnLYStmFt = false;
      bool MVIIkOpFUh = false;
      bool qLpGEbaFIC = false;
      bool UCoqzSUdpF = false;
      bool OskSHmCpuS = false;
      bool aGnmqZhhHw = false;
      bool qneDZKgRJR = false;
      bool sLOPSyZWzQ = false;
      bool ZjKobONBzZ = false;
      bool jmVsNaNgsw = false;
      bool GkgcZOqYTs = false;
      bool zKhpWHdpbK = false;
      bool xeRjhFoUBr = false;
      bool jsdCJoCgOM = false;
      bool XnhFaZFfWG = false;
      bool wXEizbSxuT = false;
      bool YVWbIFgfgh = false;
      bool EwaOmIxMIH = false;
      bool AJyuuMnuCh = false;
      string jXUBRnlPEd;
      string gNhoAojgHY;
      string gXFgFikEBR;
      string hNrJqIyGkG;
      string VqxoJateQh;
      string oTbpeMSnwi;
      string CgOluTUXxq;
      string pcuADpZQXd;
      string oFmScOkCee;
      string pKKCDobLso;
      string GVgZqLYamo;
      string lCQRtwsuJg;
      string hRsSBfSsxO;
      string NNWhXSXNaQ;
      string oKPornsrHG;
      string ufCMHnbOYL;
      string rfdYbXeisb;
      string YeUrqXCDaM;
      string lZkpSgLJZj;
      string pJoPXJfqNT;
      if(jXUBRnlPEd == GVgZqLYamo){IKzfhQLJUi = true;}
      else if(GVgZqLYamo == jXUBRnlPEd){jmVsNaNgsw = true;}
      if(gNhoAojgHY == lCQRtwsuJg){kXnLYStmFt = true;}
      else if(lCQRtwsuJg == gNhoAojgHY){GkgcZOqYTs = true;}
      if(gXFgFikEBR == hRsSBfSsxO){MVIIkOpFUh = true;}
      else if(hRsSBfSsxO == gXFgFikEBR){zKhpWHdpbK = true;}
      if(hNrJqIyGkG == NNWhXSXNaQ){qLpGEbaFIC = true;}
      else if(NNWhXSXNaQ == hNrJqIyGkG){xeRjhFoUBr = true;}
      if(VqxoJateQh == oKPornsrHG){UCoqzSUdpF = true;}
      else if(oKPornsrHG == VqxoJateQh){jsdCJoCgOM = true;}
      if(oTbpeMSnwi == ufCMHnbOYL){OskSHmCpuS = true;}
      else if(ufCMHnbOYL == oTbpeMSnwi){XnhFaZFfWG = true;}
      if(CgOluTUXxq == rfdYbXeisb){aGnmqZhhHw = true;}
      else if(rfdYbXeisb == CgOluTUXxq){wXEizbSxuT = true;}
      if(pcuADpZQXd == YeUrqXCDaM){qneDZKgRJR = true;}
      if(oFmScOkCee == lZkpSgLJZj){sLOPSyZWzQ = true;}
      if(pKKCDobLso == pJoPXJfqNT){ZjKobONBzZ = true;}
      while(YeUrqXCDaM == pcuADpZQXd){YVWbIFgfgh = true;}
      while(lZkpSgLJZj == lZkpSgLJZj){EwaOmIxMIH = true;}
      while(pJoPXJfqNT == pJoPXJfqNT){AJyuuMnuCh = true;}
      if(IKzfhQLJUi == true){IKzfhQLJUi = false;}
      if(kXnLYStmFt == true){kXnLYStmFt = false;}
      if(MVIIkOpFUh == true){MVIIkOpFUh = false;}
      if(qLpGEbaFIC == true){qLpGEbaFIC = false;}
      if(UCoqzSUdpF == true){UCoqzSUdpF = false;}
      if(OskSHmCpuS == true){OskSHmCpuS = false;}
      if(aGnmqZhhHw == true){aGnmqZhhHw = false;}
      if(qneDZKgRJR == true){qneDZKgRJR = false;}
      if(sLOPSyZWzQ == true){sLOPSyZWzQ = false;}
      if(ZjKobONBzZ == true){ZjKobONBzZ = false;}
      if(jmVsNaNgsw == true){jmVsNaNgsw = false;}
      if(GkgcZOqYTs == true){GkgcZOqYTs = false;}
      if(zKhpWHdpbK == true){zKhpWHdpbK = false;}
      if(xeRjhFoUBr == true){xeRjhFoUBr = false;}
      if(jsdCJoCgOM == true){jsdCJoCgOM = false;}
      if(XnhFaZFfWG == true){XnhFaZFfWG = false;}
      if(wXEizbSxuT == true){wXEizbSxuT = false;}
      if(YVWbIFgfgh == true){YVWbIFgfgh = false;}
      if(EwaOmIxMIH == true){EwaOmIxMIH = false;}
      if(AJyuuMnuCh == true){AJyuuMnuCh = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZZOCODRAZH
{ 
  void SjKUiXkuVM()
  { 
      bool rORQVHLtbH = false;
      bool HmLlnhSGcG = false;
      bool ZpJKwZMnbO = false;
      bool akqbPEoYMo = false;
      bool zeXzffhUTr = false;
      bool sUlPsBFGTu = false;
      bool uXRMfWuiWi = false;
      bool stsnHuRJdQ = false;
      bool rqUjVGzbiN = false;
      bool zhFRsbMKSJ = false;
      bool kyyjyzaxHx = false;
      bool iubDDEChpZ = false;
      bool XSAIjKnbuf = false;
      bool hGqUesrkkW = false;
      bool eBTkdEGBNn = false;
      bool YeUslRyoAn = false;
      bool KyCbPbEHXJ = false;
      bool LQwStVzOee = false;
      bool ElNGGDlyiN = false;
      bool uCkxMfzrVB = false;
      string DAHDKXieRZ;
      string ieesmEhRgO;
      string zwDcPBjgsW;
      string ZgDCtWrybx;
      string wIJAgXdRyU;
      string uWzQcHwdmI;
      string mBltzBbbyP;
      string MipjSplDwr;
      string WShYbCgmZl;
      string aNLSKtWPAe;
      string wCShHMgPxl;
      string UshukTTNSx;
      string UnBMozWXZX;
      string TVCRuUAAQP;
      string VRonRheQgp;
      string aIxDKHAGsI;
      string oJpZRypQzQ;
      string TSwCaCzNtH;
      string TGpiwLYzHn;
      string TsGIaLbOVS;
      if(DAHDKXieRZ == wCShHMgPxl){rORQVHLtbH = true;}
      else if(wCShHMgPxl == DAHDKXieRZ){kyyjyzaxHx = true;}
      if(ieesmEhRgO == UshukTTNSx){HmLlnhSGcG = true;}
      else if(UshukTTNSx == ieesmEhRgO){iubDDEChpZ = true;}
      if(zwDcPBjgsW == UnBMozWXZX){ZpJKwZMnbO = true;}
      else if(UnBMozWXZX == zwDcPBjgsW){XSAIjKnbuf = true;}
      if(ZgDCtWrybx == TVCRuUAAQP){akqbPEoYMo = true;}
      else if(TVCRuUAAQP == ZgDCtWrybx){hGqUesrkkW = true;}
      if(wIJAgXdRyU == VRonRheQgp){zeXzffhUTr = true;}
      else if(VRonRheQgp == wIJAgXdRyU){eBTkdEGBNn = true;}
      if(uWzQcHwdmI == aIxDKHAGsI){sUlPsBFGTu = true;}
      else if(aIxDKHAGsI == uWzQcHwdmI){YeUslRyoAn = true;}
      if(mBltzBbbyP == oJpZRypQzQ){uXRMfWuiWi = true;}
      else if(oJpZRypQzQ == mBltzBbbyP){KyCbPbEHXJ = true;}
      if(MipjSplDwr == TSwCaCzNtH){stsnHuRJdQ = true;}
      if(WShYbCgmZl == TGpiwLYzHn){rqUjVGzbiN = true;}
      if(aNLSKtWPAe == TsGIaLbOVS){zhFRsbMKSJ = true;}
      while(TSwCaCzNtH == MipjSplDwr){LQwStVzOee = true;}
      while(TGpiwLYzHn == TGpiwLYzHn){ElNGGDlyiN = true;}
      while(TsGIaLbOVS == TsGIaLbOVS){uCkxMfzrVB = true;}
      if(rORQVHLtbH == true){rORQVHLtbH = false;}
      if(HmLlnhSGcG == true){HmLlnhSGcG = false;}
      if(ZpJKwZMnbO == true){ZpJKwZMnbO = false;}
      if(akqbPEoYMo == true){akqbPEoYMo = false;}
      if(zeXzffhUTr == true){zeXzffhUTr = false;}
      if(sUlPsBFGTu == true){sUlPsBFGTu = false;}
      if(uXRMfWuiWi == true){uXRMfWuiWi = false;}
      if(stsnHuRJdQ == true){stsnHuRJdQ = false;}
      if(rqUjVGzbiN == true){rqUjVGzbiN = false;}
      if(zhFRsbMKSJ == true){zhFRsbMKSJ = false;}
      if(kyyjyzaxHx == true){kyyjyzaxHx = false;}
      if(iubDDEChpZ == true){iubDDEChpZ = false;}
      if(XSAIjKnbuf == true){XSAIjKnbuf = false;}
      if(hGqUesrkkW == true){hGqUesrkkW = false;}
      if(eBTkdEGBNn == true){eBTkdEGBNn = false;}
      if(YeUslRyoAn == true){YeUslRyoAn = false;}
      if(KyCbPbEHXJ == true){KyCbPbEHXJ = false;}
      if(LQwStVzOee == true){LQwStVzOee = false;}
      if(ElNGGDlyiN == true){ElNGGDlyiN = false;}
      if(uCkxMfzrVB == true){uCkxMfzrVB = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UIQJCFPPDP
{ 
  void odqfnScuYd()
  { 
      bool tMWBhQDUQY = false;
      bool HnSGxPjGxJ = false;
      bool otJcJVeQLW = false;
      bool sltSOmiAgf = false;
      bool HefzXqXNyw = false;
      bool RBconADsqL = false;
      bool kmJsfUsAqJ = false;
      bool fiYQCJNKFS = false;
      bool xRSOEqJFdO = false;
      bool jpJVuUVlTC = false;
      bool yXVnQhbLme = false;
      bool lsnYNSUton = false;
      bool LnWcZoZDVg = false;
      bool nbpCTZwdyV = false;
      bool PySzbqCkHb = false;
      bool AaxfqKBrOa = false;
      bool IYoFqJCkEn = false;
      bool WpwSXMVWBh = false;
      bool mowtgMeKKJ = false;
      bool YDPWNpIMIR = false;
      string OgbgummBVG;
      string KHuwmafoxl;
      string SmQuSjgLFZ;
      string dUwPddFEEV;
      string ZTtctObwSP;
      string jNITxbQfNK;
      string FbwubpLfRI;
      string VbkaLzrmBD;
      string IksopPLcRC;
      string NZpsPyXyBE;
      string DudJtHcgHx;
      string iceJCThHFL;
      string ehNPtulrLM;
      string bslxAMGith;
      string mdUUSktfpu;
      string PQsoYUgOPy;
      string VkVtVopgJo;
      string NzMXYAMswI;
      string AGBPxLAwXf;
      string eqTuLBHZzc;
      if(OgbgummBVG == DudJtHcgHx){tMWBhQDUQY = true;}
      else if(DudJtHcgHx == OgbgummBVG){yXVnQhbLme = true;}
      if(KHuwmafoxl == iceJCThHFL){HnSGxPjGxJ = true;}
      else if(iceJCThHFL == KHuwmafoxl){lsnYNSUton = true;}
      if(SmQuSjgLFZ == ehNPtulrLM){otJcJVeQLW = true;}
      else if(ehNPtulrLM == SmQuSjgLFZ){LnWcZoZDVg = true;}
      if(dUwPddFEEV == bslxAMGith){sltSOmiAgf = true;}
      else if(bslxAMGith == dUwPddFEEV){nbpCTZwdyV = true;}
      if(ZTtctObwSP == mdUUSktfpu){HefzXqXNyw = true;}
      else if(mdUUSktfpu == ZTtctObwSP){PySzbqCkHb = true;}
      if(jNITxbQfNK == PQsoYUgOPy){RBconADsqL = true;}
      else if(PQsoYUgOPy == jNITxbQfNK){AaxfqKBrOa = true;}
      if(FbwubpLfRI == VkVtVopgJo){kmJsfUsAqJ = true;}
      else if(VkVtVopgJo == FbwubpLfRI){IYoFqJCkEn = true;}
      if(VbkaLzrmBD == NzMXYAMswI){fiYQCJNKFS = true;}
      if(IksopPLcRC == AGBPxLAwXf){xRSOEqJFdO = true;}
      if(NZpsPyXyBE == eqTuLBHZzc){jpJVuUVlTC = true;}
      while(NzMXYAMswI == VbkaLzrmBD){WpwSXMVWBh = true;}
      while(AGBPxLAwXf == AGBPxLAwXf){mowtgMeKKJ = true;}
      while(eqTuLBHZzc == eqTuLBHZzc){YDPWNpIMIR = true;}
      if(tMWBhQDUQY == true){tMWBhQDUQY = false;}
      if(HnSGxPjGxJ == true){HnSGxPjGxJ = false;}
      if(otJcJVeQLW == true){otJcJVeQLW = false;}
      if(sltSOmiAgf == true){sltSOmiAgf = false;}
      if(HefzXqXNyw == true){HefzXqXNyw = false;}
      if(RBconADsqL == true){RBconADsqL = false;}
      if(kmJsfUsAqJ == true){kmJsfUsAqJ = false;}
      if(fiYQCJNKFS == true){fiYQCJNKFS = false;}
      if(xRSOEqJFdO == true){xRSOEqJFdO = false;}
      if(jpJVuUVlTC == true){jpJVuUVlTC = false;}
      if(yXVnQhbLme == true){yXVnQhbLme = false;}
      if(lsnYNSUton == true){lsnYNSUton = false;}
      if(LnWcZoZDVg == true){LnWcZoZDVg = false;}
      if(nbpCTZwdyV == true){nbpCTZwdyV = false;}
      if(PySzbqCkHb == true){PySzbqCkHb = false;}
      if(AaxfqKBrOa == true){AaxfqKBrOa = false;}
      if(IYoFqJCkEn == true){IYoFqJCkEn = false;}
      if(WpwSXMVWBh == true){WpwSXMVWBh = false;}
      if(mowtgMeKKJ == true){mowtgMeKKJ = false;}
      if(YDPWNpIMIR == true){YDPWNpIMIR = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AXVYVFXIXM
{ 
  void ExIJPNUfLo()
  { 
      bool QZGGPCUUon = false;
      bool KqQzjxiQxo = false;
      bool DJRWmaDZhp = false;
      bool BKOSAbLeZB = false;
      bool LYjKHgIFZM = false;
      bool zMOpnHHiSo = false;
      bool ztmymAcxyH = false;
      bool kDquceYGPG = false;
      bool RyAUpnuXes = false;
      bool TbMjBQklXH = false;
      bool DLGRawUTNI = false;
      bool hATLWBtRHy = false;
      bool EEREnJqGtL = false;
      bool sliHCGqhst = false;
      bool SbKVLSHAMA = false;
      bool WTeZCysCdR = false;
      bool EOYBPuRLJQ = false;
      bool JLYMectusU = false;
      bool BqssoZHIFE = false;
      bool MoEaoECIGP = false;
      string dbhBrYHaFq;
      string tIqZQbrrPV;
      string MnYxudubVi;
      string ngulhaJrBs;
      string TpCswdlTXw;
      string eNiRdoDjGN;
      string cfEWCNMEro;
      string RApVCWleBQ;
      string rbDYEZxcIr;
      string XZlmQmkeqi;
      string QhahddWMPH;
      string DXioVAcfRb;
      string PjuzRrODFO;
      string lGnRzKAUuZ;
      string CkNBnApwRP;
      string KFMOSglbQk;
      string PTGBVxiQoL;
      string zHjrZeWtKR;
      string BSkpEgsZzS;
      string XYEJKidLfq;
      if(dbhBrYHaFq == QhahddWMPH){QZGGPCUUon = true;}
      else if(QhahddWMPH == dbhBrYHaFq){DLGRawUTNI = true;}
      if(tIqZQbrrPV == DXioVAcfRb){KqQzjxiQxo = true;}
      else if(DXioVAcfRb == tIqZQbrrPV){hATLWBtRHy = true;}
      if(MnYxudubVi == PjuzRrODFO){DJRWmaDZhp = true;}
      else if(PjuzRrODFO == MnYxudubVi){EEREnJqGtL = true;}
      if(ngulhaJrBs == lGnRzKAUuZ){BKOSAbLeZB = true;}
      else if(lGnRzKAUuZ == ngulhaJrBs){sliHCGqhst = true;}
      if(TpCswdlTXw == CkNBnApwRP){LYjKHgIFZM = true;}
      else if(CkNBnApwRP == TpCswdlTXw){SbKVLSHAMA = true;}
      if(eNiRdoDjGN == KFMOSglbQk){zMOpnHHiSo = true;}
      else if(KFMOSglbQk == eNiRdoDjGN){WTeZCysCdR = true;}
      if(cfEWCNMEro == PTGBVxiQoL){ztmymAcxyH = true;}
      else if(PTGBVxiQoL == cfEWCNMEro){EOYBPuRLJQ = true;}
      if(RApVCWleBQ == zHjrZeWtKR){kDquceYGPG = true;}
      if(rbDYEZxcIr == BSkpEgsZzS){RyAUpnuXes = true;}
      if(XZlmQmkeqi == XYEJKidLfq){TbMjBQklXH = true;}
      while(zHjrZeWtKR == RApVCWleBQ){JLYMectusU = true;}
      while(BSkpEgsZzS == BSkpEgsZzS){BqssoZHIFE = true;}
      while(XYEJKidLfq == XYEJKidLfq){MoEaoECIGP = true;}
      if(QZGGPCUUon == true){QZGGPCUUon = false;}
      if(KqQzjxiQxo == true){KqQzjxiQxo = false;}
      if(DJRWmaDZhp == true){DJRWmaDZhp = false;}
      if(BKOSAbLeZB == true){BKOSAbLeZB = false;}
      if(LYjKHgIFZM == true){LYjKHgIFZM = false;}
      if(zMOpnHHiSo == true){zMOpnHHiSo = false;}
      if(ztmymAcxyH == true){ztmymAcxyH = false;}
      if(kDquceYGPG == true){kDquceYGPG = false;}
      if(RyAUpnuXes == true){RyAUpnuXes = false;}
      if(TbMjBQklXH == true){TbMjBQklXH = false;}
      if(DLGRawUTNI == true){DLGRawUTNI = false;}
      if(hATLWBtRHy == true){hATLWBtRHy = false;}
      if(EEREnJqGtL == true){EEREnJqGtL = false;}
      if(sliHCGqhst == true){sliHCGqhst = false;}
      if(SbKVLSHAMA == true){SbKVLSHAMA = false;}
      if(WTeZCysCdR == true){WTeZCysCdR = false;}
      if(EOYBPuRLJQ == true){EOYBPuRLJQ = false;}
      if(JLYMectusU == true){JLYMectusU = false;}
      if(BqssoZHIFE == true){BqssoZHIFE = false;}
      if(MoEaoECIGP == true){MoEaoECIGP = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class YCXFLVTSRT
{ 
  void BrxIreAXDk()
  { 
      bool cDLdqoXfGl = false;
      bool eNunMuAntg = false;
      bool sLpXThDlVp = false;
      bool pLoIUXuXtK = false;
      bool CODPchLCaP = false;
      bool rPmOPRMnKI = false;
      bool lkKQuTYmSn = false;
      bool zbtiddHatl = false;
      bool WBLGyaXZsD = false;
      bool LHNkYREmTJ = false;
      bool IYimPQUDjk = false;
      bool JksXljZESU = false;
      bool uytPbrNJKM = false;
      bool ZUHkcySYpg = false;
      bool FToHsaNpyn = false;
      bool rAUyOKeSKW = false;
      bool CLsXKocItn = false;
      bool RBNkOaIlVB = false;
      bool dXKVUZzJWS = false;
      bool SzNSetaRwE = false;
      string HuqZlFCYkR;
      string RBIcbVoyOg;
      string GpHgqssRlX;
      string tYajOIelxD;
      string qjWpwTLsEB;
      string FADUBRmKis;
      string LklNXsaXOS;
      string qCBKpVSRkX;
      string hANkRtVGix;
      string UUDErGyJRD;
      string rmekJDzrde;
      string yVYLKKWAuc;
      string XUnQyWzwSj;
      string XJzgetUcaO;
      string ucbDyzuqtg;
      string ixNofXzNlm;
      string DCZtHoVrfP;
      string otZghtrkRy;
      string aHCkAZuKBH;
      string uHpmLrDgwJ;
      if(HuqZlFCYkR == rmekJDzrde){cDLdqoXfGl = true;}
      else if(rmekJDzrde == HuqZlFCYkR){IYimPQUDjk = true;}
      if(RBIcbVoyOg == yVYLKKWAuc){eNunMuAntg = true;}
      else if(yVYLKKWAuc == RBIcbVoyOg){JksXljZESU = true;}
      if(GpHgqssRlX == XUnQyWzwSj){sLpXThDlVp = true;}
      else if(XUnQyWzwSj == GpHgqssRlX){uytPbrNJKM = true;}
      if(tYajOIelxD == XJzgetUcaO){pLoIUXuXtK = true;}
      else if(XJzgetUcaO == tYajOIelxD){ZUHkcySYpg = true;}
      if(qjWpwTLsEB == ucbDyzuqtg){CODPchLCaP = true;}
      else if(ucbDyzuqtg == qjWpwTLsEB){FToHsaNpyn = true;}
      if(FADUBRmKis == ixNofXzNlm){rPmOPRMnKI = true;}
      else if(ixNofXzNlm == FADUBRmKis){rAUyOKeSKW = true;}
      if(LklNXsaXOS == DCZtHoVrfP){lkKQuTYmSn = true;}
      else if(DCZtHoVrfP == LklNXsaXOS){CLsXKocItn = true;}
      if(qCBKpVSRkX == otZghtrkRy){zbtiddHatl = true;}
      if(hANkRtVGix == aHCkAZuKBH){WBLGyaXZsD = true;}
      if(UUDErGyJRD == uHpmLrDgwJ){LHNkYREmTJ = true;}
      while(otZghtrkRy == qCBKpVSRkX){RBNkOaIlVB = true;}
      while(aHCkAZuKBH == aHCkAZuKBH){dXKVUZzJWS = true;}
      while(uHpmLrDgwJ == uHpmLrDgwJ){SzNSetaRwE = true;}
      if(cDLdqoXfGl == true){cDLdqoXfGl = false;}
      if(eNunMuAntg == true){eNunMuAntg = false;}
      if(sLpXThDlVp == true){sLpXThDlVp = false;}
      if(pLoIUXuXtK == true){pLoIUXuXtK = false;}
      if(CODPchLCaP == true){CODPchLCaP = false;}
      if(rPmOPRMnKI == true){rPmOPRMnKI = false;}
      if(lkKQuTYmSn == true){lkKQuTYmSn = false;}
      if(zbtiddHatl == true){zbtiddHatl = false;}
      if(WBLGyaXZsD == true){WBLGyaXZsD = false;}
      if(LHNkYREmTJ == true){LHNkYREmTJ = false;}
      if(IYimPQUDjk == true){IYimPQUDjk = false;}
      if(JksXljZESU == true){JksXljZESU = false;}
      if(uytPbrNJKM == true){uytPbrNJKM = false;}
      if(ZUHkcySYpg == true){ZUHkcySYpg = false;}
      if(FToHsaNpyn == true){FToHsaNpyn = false;}
      if(rAUyOKeSKW == true){rAUyOKeSKW = false;}
      if(CLsXKocItn == true){CLsXKocItn = false;}
      if(RBNkOaIlVB == true){RBNkOaIlVB = false;}
      if(dXKVUZzJWS == true){dXKVUZzJWS = false;}
      if(SzNSetaRwE == true){SzNSetaRwE = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FRJJOKETJD
{ 
  void zjGbnjLCsl()
  { 
      bool cQMHXoEVJj = false;
      bool quAhkKSHMK = false;
      bool pRDtZZGWiO = false;
      bool zsubUApMTk = false;
      bool YotzieBKak = false;
      bool EpKNTLNShy = false;
      bool hhlukxzYNj = false;
      bool TGnakBWEKt = false;
      bool YuuyJJigHI = false;
      bool mXeVCqDyNk = false;
      bool fcxqQxBicS = false;
      bool oJHmZllmlb = false;
      bool ORQEwwbSYt = false;
      bool xXpZOwyEgL = false;
      bool HzEoTdPqwQ = false;
      bool EPMlWNLrtU = false;
      bool ViKiUQThYd = false;
      bool mhGZTPNbkS = false;
      bool fkWIphKDDB = false;
      bool jGJMpYnVTC = false;
      string EfKCYKciVy;
      string YajEGrUOJV;
      string foOJjpXzuY;
      string XnpgkmLBjC;
      string ramTNTABwG;
      string QwzeMORBSh;
      string lcnxZkkCTS;
      string gSeHdFCtuQ;
      string oVDoHboKUW;
      string VKiBPOmpsc;
      string hyhcZyQqBH;
      string MWFCrPZyox;
      string uZhwxWjcqO;
      string SLARhqnZPJ;
      string LxJNKzEgIz;
      string onBwhHPlfD;
      string NmzlnAVmTB;
      string bAielxQUkx;
      string RDTqZdKEjZ;
      string aBsMuXxWYJ;
      if(EfKCYKciVy == hyhcZyQqBH){cQMHXoEVJj = true;}
      else if(hyhcZyQqBH == EfKCYKciVy){fcxqQxBicS = true;}
      if(YajEGrUOJV == MWFCrPZyox){quAhkKSHMK = true;}
      else if(MWFCrPZyox == YajEGrUOJV){oJHmZllmlb = true;}
      if(foOJjpXzuY == uZhwxWjcqO){pRDtZZGWiO = true;}
      else if(uZhwxWjcqO == foOJjpXzuY){ORQEwwbSYt = true;}
      if(XnpgkmLBjC == SLARhqnZPJ){zsubUApMTk = true;}
      else if(SLARhqnZPJ == XnpgkmLBjC){xXpZOwyEgL = true;}
      if(ramTNTABwG == LxJNKzEgIz){YotzieBKak = true;}
      else if(LxJNKzEgIz == ramTNTABwG){HzEoTdPqwQ = true;}
      if(QwzeMORBSh == onBwhHPlfD){EpKNTLNShy = true;}
      else if(onBwhHPlfD == QwzeMORBSh){EPMlWNLrtU = true;}
      if(lcnxZkkCTS == NmzlnAVmTB){hhlukxzYNj = true;}
      else if(NmzlnAVmTB == lcnxZkkCTS){ViKiUQThYd = true;}
      if(gSeHdFCtuQ == bAielxQUkx){TGnakBWEKt = true;}
      if(oVDoHboKUW == RDTqZdKEjZ){YuuyJJigHI = true;}
      if(VKiBPOmpsc == aBsMuXxWYJ){mXeVCqDyNk = true;}
      while(bAielxQUkx == gSeHdFCtuQ){mhGZTPNbkS = true;}
      while(RDTqZdKEjZ == RDTqZdKEjZ){fkWIphKDDB = true;}
      while(aBsMuXxWYJ == aBsMuXxWYJ){jGJMpYnVTC = true;}
      if(cQMHXoEVJj == true){cQMHXoEVJj = false;}
      if(quAhkKSHMK == true){quAhkKSHMK = false;}
      if(pRDtZZGWiO == true){pRDtZZGWiO = false;}
      if(zsubUApMTk == true){zsubUApMTk = false;}
      if(YotzieBKak == true){YotzieBKak = false;}
      if(EpKNTLNShy == true){EpKNTLNShy = false;}
      if(hhlukxzYNj == true){hhlukxzYNj = false;}
      if(TGnakBWEKt == true){TGnakBWEKt = false;}
      if(YuuyJJigHI == true){YuuyJJigHI = false;}
      if(mXeVCqDyNk == true){mXeVCqDyNk = false;}
      if(fcxqQxBicS == true){fcxqQxBicS = false;}
      if(oJHmZllmlb == true){oJHmZllmlb = false;}
      if(ORQEwwbSYt == true){ORQEwwbSYt = false;}
      if(xXpZOwyEgL == true){xXpZOwyEgL = false;}
      if(HzEoTdPqwQ == true){HzEoTdPqwQ = false;}
      if(EPMlWNLrtU == true){EPMlWNLrtU = false;}
      if(ViKiUQThYd == true){ViKiUQThYd = false;}
      if(mhGZTPNbkS == true){mhGZTPNbkS = false;}
      if(fkWIphKDDB == true){fkWIphKDDB = false;}
      if(jGJMpYnVTC == true){jGJMpYnVTC = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KBENWDQRRH
{ 
  void DxirBFUuNq()
  { 
      bool oWMsbcCeeo = false;
      bool qKsRwcQwFy = false;
      bool lxqMXNhAmB = false;
      bool ufJjAzsDfH = false;
      bool hpMoJaDKDU = false;
      bool PTacHqezUN = false;
      bool uCrcsKBIQw = false;
      bool jMQxQukCCw = false;
      bool KWwSlaFfbJ = false;
      bool TsiRLeyiuz = false;
      bool ZtsGkDDdMM = false;
      bool FcxXDXGZqR = false;
      bool FUfGcpdykk = false;
      bool xBHDWQdogi = false;
      bool XaTPDqwKOn = false;
      bool XYrcsENldt = false;
      bool YEraPEVsAl = false;
      bool tQrItdBPDl = false;
      bool sSjxMXJeKR = false;
      bool CqLlJSirAz = false;
      string YRSWzGnOxz;
      string ZXmjNYpyco;
      string BLZAFlDitl;
      string OZDMISNNFS;
      string kqrcjJPfXr;
      string qGHSfMrSRM;
      string CEULkzUabu;
      string KcrJEJSwQu;
      string WXLBtISkfu;
      string ZuwsQfVgqj;
      string ooDbhONxNV;
      string tuCVpdsTQc;
      string gjtfUOfzsX;
      string mlFgGgTiSf;
      string tUWJBeraCl;
      string mCPNCEuXHS;
      string hNDpiHpVOJ;
      string KJAkiMMjdp;
      string WFxHkjVrdB;
      string kwbAxgQfat;
      if(YRSWzGnOxz == ooDbhONxNV){oWMsbcCeeo = true;}
      else if(ooDbhONxNV == YRSWzGnOxz){ZtsGkDDdMM = true;}
      if(ZXmjNYpyco == tuCVpdsTQc){qKsRwcQwFy = true;}
      else if(tuCVpdsTQc == ZXmjNYpyco){FcxXDXGZqR = true;}
      if(BLZAFlDitl == gjtfUOfzsX){lxqMXNhAmB = true;}
      else if(gjtfUOfzsX == BLZAFlDitl){FUfGcpdykk = true;}
      if(OZDMISNNFS == mlFgGgTiSf){ufJjAzsDfH = true;}
      else if(mlFgGgTiSf == OZDMISNNFS){xBHDWQdogi = true;}
      if(kqrcjJPfXr == tUWJBeraCl){hpMoJaDKDU = true;}
      else if(tUWJBeraCl == kqrcjJPfXr){XaTPDqwKOn = true;}
      if(qGHSfMrSRM == mCPNCEuXHS){PTacHqezUN = true;}
      else if(mCPNCEuXHS == qGHSfMrSRM){XYrcsENldt = true;}
      if(CEULkzUabu == hNDpiHpVOJ){uCrcsKBIQw = true;}
      else if(hNDpiHpVOJ == CEULkzUabu){YEraPEVsAl = true;}
      if(KcrJEJSwQu == KJAkiMMjdp){jMQxQukCCw = true;}
      if(WXLBtISkfu == WFxHkjVrdB){KWwSlaFfbJ = true;}
      if(ZuwsQfVgqj == kwbAxgQfat){TsiRLeyiuz = true;}
      while(KJAkiMMjdp == KcrJEJSwQu){tQrItdBPDl = true;}
      while(WFxHkjVrdB == WFxHkjVrdB){sSjxMXJeKR = true;}
      while(kwbAxgQfat == kwbAxgQfat){CqLlJSirAz = true;}
      if(oWMsbcCeeo == true){oWMsbcCeeo = false;}
      if(qKsRwcQwFy == true){qKsRwcQwFy = false;}
      if(lxqMXNhAmB == true){lxqMXNhAmB = false;}
      if(ufJjAzsDfH == true){ufJjAzsDfH = false;}
      if(hpMoJaDKDU == true){hpMoJaDKDU = false;}
      if(PTacHqezUN == true){PTacHqezUN = false;}
      if(uCrcsKBIQw == true){uCrcsKBIQw = false;}
      if(jMQxQukCCw == true){jMQxQukCCw = false;}
      if(KWwSlaFfbJ == true){KWwSlaFfbJ = false;}
      if(TsiRLeyiuz == true){TsiRLeyiuz = false;}
      if(ZtsGkDDdMM == true){ZtsGkDDdMM = false;}
      if(FcxXDXGZqR == true){FcxXDXGZqR = false;}
      if(FUfGcpdykk == true){FUfGcpdykk = false;}
      if(xBHDWQdogi == true){xBHDWQdogi = false;}
      if(XaTPDqwKOn == true){XaTPDqwKOn = false;}
      if(XYrcsENldt == true){XYrcsENldt = false;}
      if(YEraPEVsAl == true){YEraPEVsAl = false;}
      if(tQrItdBPDl == true){tQrItdBPDl = false;}
      if(sSjxMXJeKR == true){sSjxMXJeKR = false;}
      if(CqLlJSirAz == true){CqLlJSirAz = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class RVFNQCURIM
{ 
  void WtqULPlXlb()
  { 
      bool fmrNiIfgzI = false;
      bool yoQwmEBrMS = false;
      bool YQcbFYwQqZ = false;
      bool LAunmmWDto = false;
      bool oVHaDHwFyd = false;
      bool fNcqWPKPIK = false;
      bool MOpZwxtcJA = false;
      bool gxyWdCJBYx = false;
      bool bqSMtyXDRX = false;
      bool bGuicafhdF = false;
      bool NTwxxhOFNT = false;
      bool FIqKhVucey = false;
      bool YpLkXZCOtD = false;
      bool WbXSjItUPM = false;
      bool iSpxRTPkBH = false;
      bool eCCffWHKZW = false;
      bool fALKwdkYZZ = false;
      bool aaMSOGANTE = false;
      bool KQNmYUYqlN = false;
      bool AiusfiHeBm = false;
      string scmjhNjNTA;
      string bNcFbUwkOa;
      string RNIlzurLXj;
      string pmJcoLXXwr;
      string waTbIgBWEj;
      string coqtjfTAjm;
      string VXojKnEzPP;
      string wXoVdptgdR;
      string sUGmGAwPsJ;
      string qWmIWYkjRT;
      string cBGnnEzUPs;
      string lmWRsAQCcJ;
      string roGGXZjfzF;
      string EKOKorNSzC;
      string BazzWEzUOV;
      string HiHHanuAQN;
      string TSVKobXMcB;
      string lyCZIRoGiT;
      string ZPRcyVCGjB;
      string wNClihFoKu;
      if(scmjhNjNTA == cBGnnEzUPs){fmrNiIfgzI = true;}
      else if(cBGnnEzUPs == scmjhNjNTA){NTwxxhOFNT = true;}
      if(bNcFbUwkOa == lmWRsAQCcJ){yoQwmEBrMS = true;}
      else if(lmWRsAQCcJ == bNcFbUwkOa){FIqKhVucey = true;}
      if(RNIlzurLXj == roGGXZjfzF){YQcbFYwQqZ = true;}
      else if(roGGXZjfzF == RNIlzurLXj){YpLkXZCOtD = true;}
      if(pmJcoLXXwr == EKOKorNSzC){LAunmmWDto = true;}
      else if(EKOKorNSzC == pmJcoLXXwr){WbXSjItUPM = true;}
      if(waTbIgBWEj == BazzWEzUOV){oVHaDHwFyd = true;}
      else if(BazzWEzUOV == waTbIgBWEj){iSpxRTPkBH = true;}
      if(coqtjfTAjm == HiHHanuAQN){fNcqWPKPIK = true;}
      else if(HiHHanuAQN == coqtjfTAjm){eCCffWHKZW = true;}
      if(VXojKnEzPP == TSVKobXMcB){MOpZwxtcJA = true;}
      else if(TSVKobXMcB == VXojKnEzPP){fALKwdkYZZ = true;}
      if(wXoVdptgdR == lyCZIRoGiT){gxyWdCJBYx = true;}
      if(sUGmGAwPsJ == ZPRcyVCGjB){bqSMtyXDRX = true;}
      if(qWmIWYkjRT == wNClihFoKu){bGuicafhdF = true;}
      while(lyCZIRoGiT == wXoVdptgdR){aaMSOGANTE = true;}
      while(ZPRcyVCGjB == ZPRcyVCGjB){KQNmYUYqlN = true;}
      while(wNClihFoKu == wNClihFoKu){AiusfiHeBm = true;}
      if(fmrNiIfgzI == true){fmrNiIfgzI = false;}
      if(yoQwmEBrMS == true){yoQwmEBrMS = false;}
      if(YQcbFYwQqZ == true){YQcbFYwQqZ = false;}
      if(LAunmmWDto == true){LAunmmWDto = false;}
      if(oVHaDHwFyd == true){oVHaDHwFyd = false;}
      if(fNcqWPKPIK == true){fNcqWPKPIK = false;}
      if(MOpZwxtcJA == true){MOpZwxtcJA = false;}
      if(gxyWdCJBYx == true){gxyWdCJBYx = false;}
      if(bqSMtyXDRX == true){bqSMtyXDRX = false;}
      if(bGuicafhdF == true){bGuicafhdF = false;}
      if(NTwxxhOFNT == true){NTwxxhOFNT = false;}
      if(FIqKhVucey == true){FIqKhVucey = false;}
      if(YpLkXZCOtD == true){YpLkXZCOtD = false;}
      if(WbXSjItUPM == true){WbXSjItUPM = false;}
      if(iSpxRTPkBH == true){iSpxRTPkBH = false;}
      if(eCCffWHKZW == true){eCCffWHKZW = false;}
      if(fALKwdkYZZ == true){fALKwdkYZZ = false;}
      if(aaMSOGANTE == true){aaMSOGANTE = false;}
      if(KQNmYUYqlN == true){KQNmYUYqlN = false;}
      if(AiusfiHeBm == true){AiusfiHeBm = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CXMQTSXCUO
{ 
  void oDOHWYfxCW()
  { 
      bool TDyEBmZaFU = false;
      bool jysDSxGzXq = false;
      bool GwNfpdOQnD = false;
      bool OcVuqrPCOb = false;
      bool QjNpQImgtg = false;
      bool EpLtZXpSOW = false;
      bool XJlBFHbxMZ = false;
      bool gEjuaWhXOg = false;
      bool qhWHzYQbVT = false;
      bool CwnrEHxLjI = false;
      bool goUTzAFQTD = false;
      bool oSDgfpleCg = false;
      bool pAbrXkQrqS = false;
      bool YWmFzBNCHy = false;
      bool SAIRejrGLO = false;
      bool USIDSgNbuO = false;
      bool iMZAdRexgd = false;
      bool biWENorJMn = false;
      bool rKgbxGOimS = false;
      bool nkGsOFaHnN = false;
      string AAqLSFPoKW;
      string FoRzcWQsoo;
      string tTuOFJsEQE;
      string musoCsNOWl;
      string zlKttbMtsy;
      string HrwfngEbGI;
      string MLYGjJwcxi;
      string JCVDpIJATc;
      string xzKRwuzMAV;
      string nkINZmMEKj;
      string fiwKnFVspe;
      string WEIcLNAHJo;
      string KicauYsIcC;
      string wFojxNCAcp;
      string YFWUyibeBM;
      string mEtVYwGVPP;
      string LNPmiRPXfx;
      string KAdSPEMHbL;
      string wOGQPqKTKA;
      string HfkNdLpilg;
      if(AAqLSFPoKW == fiwKnFVspe){TDyEBmZaFU = true;}
      else if(fiwKnFVspe == AAqLSFPoKW){goUTzAFQTD = true;}
      if(FoRzcWQsoo == WEIcLNAHJo){jysDSxGzXq = true;}
      else if(WEIcLNAHJo == FoRzcWQsoo){oSDgfpleCg = true;}
      if(tTuOFJsEQE == KicauYsIcC){GwNfpdOQnD = true;}
      else if(KicauYsIcC == tTuOFJsEQE){pAbrXkQrqS = true;}
      if(musoCsNOWl == wFojxNCAcp){OcVuqrPCOb = true;}
      else if(wFojxNCAcp == musoCsNOWl){YWmFzBNCHy = true;}
      if(zlKttbMtsy == YFWUyibeBM){QjNpQImgtg = true;}
      else if(YFWUyibeBM == zlKttbMtsy){SAIRejrGLO = true;}
      if(HrwfngEbGI == mEtVYwGVPP){EpLtZXpSOW = true;}
      else if(mEtVYwGVPP == HrwfngEbGI){USIDSgNbuO = true;}
      if(MLYGjJwcxi == LNPmiRPXfx){XJlBFHbxMZ = true;}
      else if(LNPmiRPXfx == MLYGjJwcxi){iMZAdRexgd = true;}
      if(JCVDpIJATc == KAdSPEMHbL){gEjuaWhXOg = true;}
      if(xzKRwuzMAV == wOGQPqKTKA){qhWHzYQbVT = true;}
      if(nkINZmMEKj == HfkNdLpilg){CwnrEHxLjI = true;}
      while(KAdSPEMHbL == JCVDpIJATc){biWENorJMn = true;}
      while(wOGQPqKTKA == wOGQPqKTKA){rKgbxGOimS = true;}
      while(HfkNdLpilg == HfkNdLpilg){nkGsOFaHnN = true;}
      if(TDyEBmZaFU == true){TDyEBmZaFU = false;}
      if(jysDSxGzXq == true){jysDSxGzXq = false;}
      if(GwNfpdOQnD == true){GwNfpdOQnD = false;}
      if(OcVuqrPCOb == true){OcVuqrPCOb = false;}
      if(QjNpQImgtg == true){QjNpQImgtg = false;}
      if(EpLtZXpSOW == true){EpLtZXpSOW = false;}
      if(XJlBFHbxMZ == true){XJlBFHbxMZ = false;}
      if(gEjuaWhXOg == true){gEjuaWhXOg = false;}
      if(qhWHzYQbVT == true){qhWHzYQbVT = false;}
      if(CwnrEHxLjI == true){CwnrEHxLjI = false;}
      if(goUTzAFQTD == true){goUTzAFQTD = false;}
      if(oSDgfpleCg == true){oSDgfpleCg = false;}
      if(pAbrXkQrqS == true){pAbrXkQrqS = false;}
      if(YWmFzBNCHy == true){YWmFzBNCHy = false;}
      if(SAIRejrGLO == true){SAIRejrGLO = false;}
      if(USIDSgNbuO == true){USIDSgNbuO = false;}
      if(iMZAdRexgd == true){iMZAdRexgd = false;}
      if(biWENorJMn == true){biWENorJMn = false;}
      if(rKgbxGOimS == true){rKgbxGOimS = false;}
      if(nkGsOFaHnN == true){nkGsOFaHnN = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CALCSVQEKC
{ 
  void HmLFaxVnNl()
  { 
      bool WVwJLHAjwq = false;
      bool JQGCLNOdZe = false;
      bool SbWIRyphix = false;
      bool oXoboaLZrr = false;
      bool pEsKoAtXUf = false;
      bool qwHSPtzmrM = false;
      bool RSBzCkbeMk = false;
      bool CnlgoNiLTQ = false;
      bool ESNDFrPWLu = false;
      bool bYheaXDtHE = false;
      bool YOKjdDKZcB = false;
      bool LmTaHzFysJ = false;
      bool UFSRIdGGGQ = false;
      bool KoDOqoHdZC = false;
      bool gpsKjnFGXb = false;
      bool SFAyiTusaz = false;
      bool wESMqViEiR = false;
      bool qbXqbeMIgU = false;
      bool YzieyEnFeX = false;
      bool sOCZFcWgSN = false;
      string XpOEWLcIEX;
      string PdsdsRbJSr;
      string lCospnJRMP;
      string jnJsXwxohc;
      string oOttFyJYMV;
      string TDkCyUBUOi;
      string BLunmaUzFw;
      string TmboCXIumq;
      string dkFqwfDHtD;
      string ubayadSFGe;
      string cJSPBgbuza;
      string LGgWEQFErH;
      string XNFBBrLdNY;
      string FDnIOURopO;
      string EHHSqntEsK;
      string XONzGODQOJ;
      string UygZVIRYIi;
      string zQOcCPFlKM;
      string xHjDApooBz;
      string RAZfsFahxi;
      if(XpOEWLcIEX == cJSPBgbuza){WVwJLHAjwq = true;}
      else if(cJSPBgbuza == XpOEWLcIEX){YOKjdDKZcB = true;}
      if(PdsdsRbJSr == LGgWEQFErH){JQGCLNOdZe = true;}
      else if(LGgWEQFErH == PdsdsRbJSr){LmTaHzFysJ = true;}
      if(lCospnJRMP == XNFBBrLdNY){SbWIRyphix = true;}
      else if(XNFBBrLdNY == lCospnJRMP){UFSRIdGGGQ = true;}
      if(jnJsXwxohc == FDnIOURopO){oXoboaLZrr = true;}
      else if(FDnIOURopO == jnJsXwxohc){KoDOqoHdZC = true;}
      if(oOttFyJYMV == EHHSqntEsK){pEsKoAtXUf = true;}
      else if(EHHSqntEsK == oOttFyJYMV){gpsKjnFGXb = true;}
      if(TDkCyUBUOi == XONzGODQOJ){qwHSPtzmrM = true;}
      else if(XONzGODQOJ == TDkCyUBUOi){SFAyiTusaz = true;}
      if(BLunmaUzFw == UygZVIRYIi){RSBzCkbeMk = true;}
      else if(UygZVIRYIi == BLunmaUzFw){wESMqViEiR = true;}
      if(TmboCXIumq == zQOcCPFlKM){CnlgoNiLTQ = true;}
      if(dkFqwfDHtD == xHjDApooBz){ESNDFrPWLu = true;}
      if(ubayadSFGe == RAZfsFahxi){bYheaXDtHE = true;}
      while(zQOcCPFlKM == TmboCXIumq){qbXqbeMIgU = true;}
      while(xHjDApooBz == xHjDApooBz){YzieyEnFeX = true;}
      while(RAZfsFahxi == RAZfsFahxi){sOCZFcWgSN = true;}
      if(WVwJLHAjwq == true){WVwJLHAjwq = false;}
      if(JQGCLNOdZe == true){JQGCLNOdZe = false;}
      if(SbWIRyphix == true){SbWIRyphix = false;}
      if(oXoboaLZrr == true){oXoboaLZrr = false;}
      if(pEsKoAtXUf == true){pEsKoAtXUf = false;}
      if(qwHSPtzmrM == true){qwHSPtzmrM = false;}
      if(RSBzCkbeMk == true){RSBzCkbeMk = false;}
      if(CnlgoNiLTQ == true){CnlgoNiLTQ = false;}
      if(ESNDFrPWLu == true){ESNDFrPWLu = false;}
      if(bYheaXDtHE == true){bYheaXDtHE = false;}
      if(YOKjdDKZcB == true){YOKjdDKZcB = false;}
      if(LmTaHzFysJ == true){LmTaHzFysJ = false;}
      if(UFSRIdGGGQ == true){UFSRIdGGGQ = false;}
      if(KoDOqoHdZC == true){KoDOqoHdZC = false;}
      if(gpsKjnFGXb == true){gpsKjnFGXb = false;}
      if(SFAyiTusaz == true){SFAyiTusaz = false;}
      if(wESMqViEiR == true){wESMqViEiR = false;}
      if(qbXqbeMIgU == true){qbXqbeMIgU = false;}
      if(YzieyEnFeX == true){YzieyEnFeX = false;}
      if(sOCZFcWgSN == true){sOCZFcWgSN = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class RWTIBOYNXO
{ 
  void ZUYYcKzsEZ()
  { 
      bool QWYptushwI = false;
      bool gRzFFIXMyo = false;
      bool lRfFCAGrFC = false;
      bool UIBHuNCals = false;
      bool cbeWXMgaQe = false;
      bool gYBoEypTmt = false;
      bool nBbsnXPqJr = false;
      bool FwZEMJlJgp = false;
      bool SfaZVuHMDw = false;
      bool WinrFazliz = false;
      bool wpQYYjzwbu = false;
      bool MglxbIZBXr = false;
      bool QDnNUWIhzl = false;
      bool JAahscOCBk = false;
      bool wKkHipxqPw = false;
      bool ByNphrBDDC = false;
      bool bRwpkMGDDz = false;
      bool sWAluFSfde = false;
      bool ZRRCkLljzB = false;
      bool aLzyJeNuVW = false;
      string BYRyebrCtA;
      string BhratHdKNT;
      string DwQOKWAZzN;
      string ZhpmXptcnP;
      string hnWfSdZlpY;
      string wAAtGFICmH;
      string aSDgaSqEDD;
      string MBVxkPZjug;
      string xNiZRYhugS;
      string hqpgnPkoFA;
      string lUimMteCIp;
      string fXHFJMicPB;
      string kuWkVUKoJV;
      string ejeuubewNW;
      string templJjjWb;
      string gIZDOpyyQq;
      string iosmLusMKg;
      string glOrHGIfyL;
      string kaRgnVJbfF;
      string dgfWmEkzep;
      if(BYRyebrCtA == lUimMteCIp){QWYptushwI = true;}
      else if(lUimMteCIp == BYRyebrCtA){wpQYYjzwbu = true;}
      if(BhratHdKNT == fXHFJMicPB){gRzFFIXMyo = true;}
      else if(fXHFJMicPB == BhratHdKNT){MglxbIZBXr = true;}
      if(DwQOKWAZzN == kuWkVUKoJV){lRfFCAGrFC = true;}
      else if(kuWkVUKoJV == DwQOKWAZzN){QDnNUWIhzl = true;}
      if(ZhpmXptcnP == ejeuubewNW){UIBHuNCals = true;}
      else if(ejeuubewNW == ZhpmXptcnP){JAahscOCBk = true;}
      if(hnWfSdZlpY == templJjjWb){cbeWXMgaQe = true;}
      else if(templJjjWb == hnWfSdZlpY){wKkHipxqPw = true;}
      if(wAAtGFICmH == gIZDOpyyQq){gYBoEypTmt = true;}
      else if(gIZDOpyyQq == wAAtGFICmH){ByNphrBDDC = true;}
      if(aSDgaSqEDD == iosmLusMKg){nBbsnXPqJr = true;}
      else if(iosmLusMKg == aSDgaSqEDD){bRwpkMGDDz = true;}
      if(MBVxkPZjug == glOrHGIfyL){FwZEMJlJgp = true;}
      if(xNiZRYhugS == kaRgnVJbfF){SfaZVuHMDw = true;}
      if(hqpgnPkoFA == dgfWmEkzep){WinrFazliz = true;}
      while(glOrHGIfyL == MBVxkPZjug){sWAluFSfde = true;}
      while(kaRgnVJbfF == kaRgnVJbfF){ZRRCkLljzB = true;}
      while(dgfWmEkzep == dgfWmEkzep){aLzyJeNuVW = true;}
      if(QWYptushwI == true){QWYptushwI = false;}
      if(gRzFFIXMyo == true){gRzFFIXMyo = false;}
      if(lRfFCAGrFC == true){lRfFCAGrFC = false;}
      if(UIBHuNCals == true){UIBHuNCals = false;}
      if(cbeWXMgaQe == true){cbeWXMgaQe = false;}
      if(gYBoEypTmt == true){gYBoEypTmt = false;}
      if(nBbsnXPqJr == true){nBbsnXPqJr = false;}
      if(FwZEMJlJgp == true){FwZEMJlJgp = false;}
      if(SfaZVuHMDw == true){SfaZVuHMDw = false;}
      if(WinrFazliz == true){WinrFazliz = false;}
      if(wpQYYjzwbu == true){wpQYYjzwbu = false;}
      if(MglxbIZBXr == true){MglxbIZBXr = false;}
      if(QDnNUWIhzl == true){QDnNUWIhzl = false;}
      if(JAahscOCBk == true){JAahscOCBk = false;}
      if(wKkHipxqPw == true){wKkHipxqPw = false;}
      if(ByNphrBDDC == true){ByNphrBDDC = false;}
      if(bRwpkMGDDz == true){bRwpkMGDDz = false;}
      if(sWAluFSfde == true){sWAluFSfde = false;}
      if(ZRRCkLljzB == true){ZRRCkLljzB = false;}
      if(aLzyJeNuVW == true){aLzyJeNuVW = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FWWTSYXOCA
{ 
  void yMJiDDiRYZ()
  { 
      bool PWsFlywyNf = false;
      bool auBaKbnERG = false;
      bool BPTafymymW = false;
      bool BCqaRYLTuQ = false;
      bool DJEKqAdSXC = false;
      bool WHtrczDSYn = false;
      bool KxLpeYDZIk = false;
      bool PUJBDqLJjf = false;
      bool UUajztHzyS = false;
      bool VstHFFyNfR = false;
      bool wDJmlCysnl = false;
      bool ntisSpWaqk = false;
      bool QEaoPWZBuy = false;
      bool rxcRiQKmxw = false;
      bool wOKLeaJBnP = false;
      bool hXWtBrbTXP = false;
      bool bwxospQRJC = false;
      bool ejctoBsOKn = false;
      bool MXNfQTaiPq = false;
      bool npPhUzQVSn = false;
      string XgQdEEZKGK;
      string kcujcLVIGw;
      string cZUjZhDBNp;
      string JrCHTEGqLf;
      string mxIDwVUQLn;
      string EQKiQyLfEV;
      string BYwPArZmPX;
      string EewaIeQQmD;
      string NmpZlsgSqV;
      string ralpdOBsDD;
      string wgBmjZTiak;
      string gusAIOiWdH;
      string KGwtedrGxt;
      string dXNiHflCoL;
      string YyrFnxglgP;
      string edPusokGIG;
      string hNlfGJhJrb;
      string cggRhMOpFh;
      string wFYhOzIRpR;
      string ueshdLLLzd;
      if(XgQdEEZKGK == wgBmjZTiak){PWsFlywyNf = true;}
      else if(wgBmjZTiak == XgQdEEZKGK){wDJmlCysnl = true;}
      if(kcujcLVIGw == gusAIOiWdH){auBaKbnERG = true;}
      else if(gusAIOiWdH == kcujcLVIGw){ntisSpWaqk = true;}
      if(cZUjZhDBNp == KGwtedrGxt){BPTafymymW = true;}
      else if(KGwtedrGxt == cZUjZhDBNp){QEaoPWZBuy = true;}
      if(JrCHTEGqLf == dXNiHflCoL){BCqaRYLTuQ = true;}
      else if(dXNiHflCoL == JrCHTEGqLf){rxcRiQKmxw = true;}
      if(mxIDwVUQLn == YyrFnxglgP){DJEKqAdSXC = true;}
      else if(YyrFnxglgP == mxIDwVUQLn){wOKLeaJBnP = true;}
      if(EQKiQyLfEV == edPusokGIG){WHtrczDSYn = true;}
      else if(edPusokGIG == EQKiQyLfEV){hXWtBrbTXP = true;}
      if(BYwPArZmPX == hNlfGJhJrb){KxLpeYDZIk = true;}
      else if(hNlfGJhJrb == BYwPArZmPX){bwxospQRJC = true;}
      if(EewaIeQQmD == cggRhMOpFh){PUJBDqLJjf = true;}
      if(NmpZlsgSqV == wFYhOzIRpR){UUajztHzyS = true;}
      if(ralpdOBsDD == ueshdLLLzd){VstHFFyNfR = true;}
      while(cggRhMOpFh == EewaIeQQmD){ejctoBsOKn = true;}
      while(wFYhOzIRpR == wFYhOzIRpR){MXNfQTaiPq = true;}
      while(ueshdLLLzd == ueshdLLLzd){npPhUzQVSn = true;}
      if(PWsFlywyNf == true){PWsFlywyNf = false;}
      if(auBaKbnERG == true){auBaKbnERG = false;}
      if(BPTafymymW == true){BPTafymymW = false;}
      if(BCqaRYLTuQ == true){BCqaRYLTuQ = false;}
      if(DJEKqAdSXC == true){DJEKqAdSXC = false;}
      if(WHtrczDSYn == true){WHtrczDSYn = false;}
      if(KxLpeYDZIk == true){KxLpeYDZIk = false;}
      if(PUJBDqLJjf == true){PUJBDqLJjf = false;}
      if(UUajztHzyS == true){UUajztHzyS = false;}
      if(VstHFFyNfR == true){VstHFFyNfR = false;}
      if(wDJmlCysnl == true){wDJmlCysnl = false;}
      if(ntisSpWaqk == true){ntisSpWaqk = false;}
      if(QEaoPWZBuy == true){QEaoPWZBuy = false;}
      if(rxcRiQKmxw == true){rxcRiQKmxw = false;}
      if(wOKLeaJBnP == true){wOKLeaJBnP = false;}
      if(hXWtBrbTXP == true){hXWtBrbTXP = false;}
      if(bwxospQRJC == true){bwxospQRJC = false;}
      if(ejctoBsOKn == true){ejctoBsOKn = false;}
      if(MXNfQTaiPq == true){MXNfQTaiPq = false;}
      if(npPhUzQVSn == true){npPhUzQVSn = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class OZPQBBAPKT
{ 
  void hbuClzXYGX()
  { 
      bool bcUTUBTgsS = false;
      bool kqyAeZInWf = false;
      bool JyKpRiZKhF = false;
      bool siJTKjGLTK = false;
      bool rkABxoozxB = false;
      bool qaETBcSeCl = false;
      bool aJzmZBhdou = false;
      bool TxTLnXyIrb = false;
      bool taMgNRdpRg = false;
      bool RaIhjilMbu = false;
      bool HLJBgtGsed = false;
      bool dKlZrzwSZI = false;
      bool WqLIcrnlbF = false;
      bool llgwUhRKzS = false;
      bool KkKnDHoioj = false;
      bool mTlSYPzAYo = false;
      bool FUQZqrcoEJ = false;
      bool gkSGykgWFV = false;
      bool DFwqSSANXF = false;
      bool iQlJNpUkxK = false;
      string SaxSKFQuUe;
      string ZcrmipQNwy;
      string hwDyRrDcEl;
      string IYaouaiyQi;
      string ZDKQbTGoAU;
      string nhWtnWWyBR;
      string CnpiVBCDrI;
      string GRwNYekmWx;
      string lOcixaBfBY;
      string DjxyflMbta;
      string RDwqOVlJVJ;
      string kjbDfnKHku;
      string zVBSFVUxgm;
      string RJrwehRkUb;
      string oqKrmTIsRr;
      string aFpzawBdlr;
      string AncirbqeYy;
      string WZlJGHLJel;
      string NaeoLLSsUi;
      string gmzbaAObAL;
      if(SaxSKFQuUe == RDwqOVlJVJ){bcUTUBTgsS = true;}
      else if(RDwqOVlJVJ == SaxSKFQuUe){HLJBgtGsed = true;}
      if(ZcrmipQNwy == kjbDfnKHku){kqyAeZInWf = true;}
      else if(kjbDfnKHku == ZcrmipQNwy){dKlZrzwSZI = true;}
      if(hwDyRrDcEl == zVBSFVUxgm){JyKpRiZKhF = true;}
      else if(zVBSFVUxgm == hwDyRrDcEl){WqLIcrnlbF = true;}
      if(IYaouaiyQi == RJrwehRkUb){siJTKjGLTK = true;}
      else if(RJrwehRkUb == IYaouaiyQi){llgwUhRKzS = true;}
      if(ZDKQbTGoAU == oqKrmTIsRr){rkABxoozxB = true;}
      else if(oqKrmTIsRr == ZDKQbTGoAU){KkKnDHoioj = true;}
      if(nhWtnWWyBR == aFpzawBdlr){qaETBcSeCl = true;}
      else if(aFpzawBdlr == nhWtnWWyBR){mTlSYPzAYo = true;}
      if(CnpiVBCDrI == AncirbqeYy){aJzmZBhdou = true;}
      else if(AncirbqeYy == CnpiVBCDrI){FUQZqrcoEJ = true;}
      if(GRwNYekmWx == WZlJGHLJel){TxTLnXyIrb = true;}
      if(lOcixaBfBY == NaeoLLSsUi){taMgNRdpRg = true;}
      if(DjxyflMbta == gmzbaAObAL){RaIhjilMbu = true;}
      while(WZlJGHLJel == GRwNYekmWx){gkSGykgWFV = true;}
      while(NaeoLLSsUi == NaeoLLSsUi){DFwqSSANXF = true;}
      while(gmzbaAObAL == gmzbaAObAL){iQlJNpUkxK = true;}
      if(bcUTUBTgsS == true){bcUTUBTgsS = false;}
      if(kqyAeZInWf == true){kqyAeZInWf = false;}
      if(JyKpRiZKhF == true){JyKpRiZKhF = false;}
      if(siJTKjGLTK == true){siJTKjGLTK = false;}
      if(rkABxoozxB == true){rkABxoozxB = false;}
      if(qaETBcSeCl == true){qaETBcSeCl = false;}
      if(aJzmZBhdou == true){aJzmZBhdou = false;}
      if(TxTLnXyIrb == true){TxTLnXyIrb = false;}
      if(taMgNRdpRg == true){taMgNRdpRg = false;}
      if(RaIhjilMbu == true){RaIhjilMbu = false;}
      if(HLJBgtGsed == true){HLJBgtGsed = false;}
      if(dKlZrzwSZI == true){dKlZrzwSZI = false;}
      if(WqLIcrnlbF == true){WqLIcrnlbF = false;}
      if(llgwUhRKzS == true){llgwUhRKzS = false;}
      if(KkKnDHoioj == true){KkKnDHoioj = false;}
      if(mTlSYPzAYo == true){mTlSYPzAYo = false;}
      if(FUQZqrcoEJ == true){FUQZqrcoEJ = false;}
      if(gkSGykgWFV == true){gkSGykgWFV = false;}
      if(DFwqSSANXF == true){DFwqSSANXF = false;}
      if(iQlJNpUkxK == true){iQlJNpUkxK = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class IQOZCZPMGR
{ 
  void MmcVeWgdof()
  { 
      bool MkHjWpLtnp = false;
      bool RBEWhyRNJg = false;
      bool QQDpGosOXn = false;
      bool bdXxzLiraM = false;
      bool tZQSDgBdqD = false;
      bool HIJjswgQcg = false;
      bool xxWANncHWV = false;
      bool PZmxmpzjmZ = false;
      bool nOWTfSTmDD = false;
      bool eROsOxDPEP = false;
      bool DsHVnhVMxD = false;
      bool wxdDsiZCTu = false;
      bool TpDSqyKsho = false;
      bool ljeXVtGBfy = false;
      bool xHoAHhmKzf = false;
      bool WBuJuNpneo = false;
      bool WbeHQMRGgz = false;
      bool bYVVxMACLJ = false;
      bool KAtBSjnUbI = false;
      bool DRVpaVbhju = false;
      string mYIxJLkeap;
      string VnjEZpKyRG;
      string OfLdalStHP;
      string xefqeWllJX;
      string FPzYLzcmfd;
      string XjVAsHNdWW;
      string rqWKrpHrYD;
      string DYntBzflSa;
      string JfNdqNJeyW;
      string IIOmeNsofU;
      string qjNcZzuazs;
      string baKuNMdrSe;
      string gctCtuJBPm;
      string GpFxjYRjEr;
      string ShfEuhySYR;
      string jAVBnOpLie;
      string osLMmkUMLo;
      string usnaMyxoGM;
      string xpbxCGUEJU;
      string hFAYCYWSRJ;
      if(mYIxJLkeap == qjNcZzuazs){MkHjWpLtnp = true;}
      else if(qjNcZzuazs == mYIxJLkeap){DsHVnhVMxD = true;}
      if(VnjEZpKyRG == baKuNMdrSe){RBEWhyRNJg = true;}
      else if(baKuNMdrSe == VnjEZpKyRG){wxdDsiZCTu = true;}
      if(OfLdalStHP == gctCtuJBPm){QQDpGosOXn = true;}
      else if(gctCtuJBPm == OfLdalStHP){TpDSqyKsho = true;}
      if(xefqeWllJX == GpFxjYRjEr){bdXxzLiraM = true;}
      else if(GpFxjYRjEr == xefqeWllJX){ljeXVtGBfy = true;}
      if(FPzYLzcmfd == ShfEuhySYR){tZQSDgBdqD = true;}
      else if(ShfEuhySYR == FPzYLzcmfd){xHoAHhmKzf = true;}
      if(XjVAsHNdWW == jAVBnOpLie){HIJjswgQcg = true;}
      else if(jAVBnOpLie == XjVAsHNdWW){WBuJuNpneo = true;}
      if(rqWKrpHrYD == osLMmkUMLo){xxWANncHWV = true;}
      else if(osLMmkUMLo == rqWKrpHrYD){WbeHQMRGgz = true;}
      if(DYntBzflSa == usnaMyxoGM){PZmxmpzjmZ = true;}
      if(JfNdqNJeyW == xpbxCGUEJU){nOWTfSTmDD = true;}
      if(IIOmeNsofU == hFAYCYWSRJ){eROsOxDPEP = true;}
      while(usnaMyxoGM == DYntBzflSa){bYVVxMACLJ = true;}
      while(xpbxCGUEJU == xpbxCGUEJU){KAtBSjnUbI = true;}
      while(hFAYCYWSRJ == hFAYCYWSRJ){DRVpaVbhju = true;}
      if(MkHjWpLtnp == true){MkHjWpLtnp = false;}
      if(RBEWhyRNJg == true){RBEWhyRNJg = false;}
      if(QQDpGosOXn == true){QQDpGosOXn = false;}
      if(bdXxzLiraM == true){bdXxzLiraM = false;}
      if(tZQSDgBdqD == true){tZQSDgBdqD = false;}
      if(HIJjswgQcg == true){HIJjswgQcg = false;}
      if(xxWANncHWV == true){xxWANncHWV = false;}
      if(PZmxmpzjmZ == true){PZmxmpzjmZ = false;}
      if(nOWTfSTmDD == true){nOWTfSTmDD = false;}
      if(eROsOxDPEP == true){eROsOxDPEP = false;}
      if(DsHVnhVMxD == true){DsHVnhVMxD = false;}
      if(wxdDsiZCTu == true){wxdDsiZCTu = false;}
      if(TpDSqyKsho == true){TpDSqyKsho = false;}
      if(ljeXVtGBfy == true){ljeXVtGBfy = false;}
      if(xHoAHhmKzf == true){xHoAHhmKzf = false;}
      if(WBuJuNpneo == true){WBuJuNpneo = false;}
      if(WbeHQMRGgz == true){WbeHQMRGgz = false;}
      if(bYVVxMACLJ == true){bYVVxMACLJ = false;}
      if(KAtBSjnUbI == true){KAtBSjnUbI = false;}
      if(DRVpaVbhju == true){DRVpaVbhju = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class MHOWGEWBDZ
{ 
  void IoyjydaUqR()
  { 
      bool jBFZOXafuj = false;
      bool ICDJYpalmP = false;
      bool lAoTJZyuZG = false;
      bool pTiKjrlyHz = false;
      bool OxcyORdhIq = false;
      bool nCYngiMlQn = false;
      bool ZfFnsnTsYg = false;
      bool DcenBnXpEq = false;
      bool wPmiSmutcA = false;
      bool fAhSYiqnFa = false;
      bool ykIbmKXjls = false;
      bool LVgYrJrWVG = false;
      bool sYPYADtLPS = false;
      bool xikdsKHKuH = false;
      bool OKIjnJSIaP = false;
      bool pTRygYUiai = false;
      bool PMUTubjDeg = false;
      bool BhaOqhQekq = false;
      bool klkzccQlwT = false;
      bool pXjLqCpnoL = false;
      string ySTalRRIDS;
      string OqtwZVuJTP;
      string crPITwhgpu;
      string qnVlUZGHke;
      string HARaSOwWGC;
      string JAoCbGCLHp;
      string jkRHaaVHaz;
      string OnXnHbaopE;
      string zLankqySJO;
      string WuzuuWhJAr;
      string rnuGmVcBuz;
      string fNVDpXJdWj;
      string bDxMcTncsj;
      string tQoyLLPUAO;
      string OxWKhIZwEW;
      string NEmQPnphcS;
      string hzHKFOYXqe;
      string IocHVefmdT;
      string tqXVWocXhX;
      string UhqhgOoMET;
      if(ySTalRRIDS == rnuGmVcBuz){jBFZOXafuj = true;}
      else if(rnuGmVcBuz == ySTalRRIDS){ykIbmKXjls = true;}
      if(OqtwZVuJTP == fNVDpXJdWj){ICDJYpalmP = true;}
      else if(fNVDpXJdWj == OqtwZVuJTP){LVgYrJrWVG = true;}
      if(crPITwhgpu == bDxMcTncsj){lAoTJZyuZG = true;}
      else if(bDxMcTncsj == crPITwhgpu){sYPYADtLPS = true;}
      if(qnVlUZGHke == tQoyLLPUAO){pTiKjrlyHz = true;}
      else if(tQoyLLPUAO == qnVlUZGHke){xikdsKHKuH = true;}
      if(HARaSOwWGC == OxWKhIZwEW){OxcyORdhIq = true;}
      else if(OxWKhIZwEW == HARaSOwWGC){OKIjnJSIaP = true;}
      if(JAoCbGCLHp == NEmQPnphcS){nCYngiMlQn = true;}
      else if(NEmQPnphcS == JAoCbGCLHp){pTRygYUiai = true;}
      if(jkRHaaVHaz == hzHKFOYXqe){ZfFnsnTsYg = true;}
      else if(hzHKFOYXqe == jkRHaaVHaz){PMUTubjDeg = true;}
      if(OnXnHbaopE == IocHVefmdT){DcenBnXpEq = true;}
      if(zLankqySJO == tqXVWocXhX){wPmiSmutcA = true;}
      if(WuzuuWhJAr == UhqhgOoMET){fAhSYiqnFa = true;}
      while(IocHVefmdT == OnXnHbaopE){BhaOqhQekq = true;}
      while(tqXVWocXhX == tqXVWocXhX){klkzccQlwT = true;}
      while(UhqhgOoMET == UhqhgOoMET){pXjLqCpnoL = true;}
      if(jBFZOXafuj == true){jBFZOXafuj = false;}
      if(ICDJYpalmP == true){ICDJYpalmP = false;}
      if(lAoTJZyuZG == true){lAoTJZyuZG = false;}
      if(pTiKjrlyHz == true){pTiKjrlyHz = false;}
      if(OxcyORdhIq == true){OxcyORdhIq = false;}
      if(nCYngiMlQn == true){nCYngiMlQn = false;}
      if(ZfFnsnTsYg == true){ZfFnsnTsYg = false;}
      if(DcenBnXpEq == true){DcenBnXpEq = false;}
      if(wPmiSmutcA == true){wPmiSmutcA = false;}
      if(fAhSYiqnFa == true){fAhSYiqnFa = false;}
      if(ykIbmKXjls == true){ykIbmKXjls = false;}
      if(LVgYrJrWVG == true){LVgYrJrWVG = false;}
      if(sYPYADtLPS == true){sYPYADtLPS = false;}
      if(xikdsKHKuH == true){xikdsKHKuH = false;}
      if(OKIjnJSIaP == true){OKIjnJSIaP = false;}
      if(pTRygYUiai == true){pTRygYUiai = false;}
      if(PMUTubjDeg == true){PMUTubjDeg = false;}
      if(BhaOqhQekq == true){BhaOqhQekq = false;}
      if(klkzccQlwT == true){klkzccQlwT = false;}
      if(pXjLqCpnoL == true){pXjLqCpnoL = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class RXNVQPBNZJ
{ 
  void nlXXGPALcm()
  { 
      bool cMYGxwbpnQ = false;
      bool BctAYpFmTx = false;
      bool SZrkJqqdFV = false;
      bool xgQsIruTSO = false;
      bool zJulcyIILC = false;
      bool qzZrcCTnwG = false;
      bool FxkRSULArg = false;
      bool ulUFxJdGIL = false;
      bool ihRCeyGYQF = false;
      bool XHXLICXcSI = false;
      bool lIQeHFdTuT = false;
      bool YNfkXWehmU = false;
      bool NmjlInOIxt = false;
      bool kAmAYaRmuY = false;
      bool TmcxgJalnk = false;
      bool WppytyYdWT = false;
      bool qUWAhXZfLw = false;
      bool FrmOAaJlFy = false;
      bool MbULXuKnFa = false;
      bool CnQbhzPcOI = false;
      string fYMnujgwlH;
      string aSIaBBmzJu;
      string ouajfmffOM;
      string hwDjEmiTtc;
      string tCQdFtbHYJ;
      string nEsodkycIH;
      string DBhZZdPYVN;
      string STFgAmewel;
      string LXGEXcCNFT;
      string oFKmYrTpEb;
      string dClKpTzMnq;
      string GByWufucCx;
      string aBTCYMISII;
      string SnqCHJluOD;
      string dpZNNmHnrr;
      string xqnHAchild;
      string KfDfYRKuOk;
      string tAOySjaFok;
      string EUaQLbzjCd;
      string aCIjsxltou;
      if(fYMnujgwlH == dClKpTzMnq){cMYGxwbpnQ = true;}
      else if(dClKpTzMnq == fYMnujgwlH){lIQeHFdTuT = true;}
      if(aSIaBBmzJu == GByWufucCx){BctAYpFmTx = true;}
      else if(GByWufucCx == aSIaBBmzJu){YNfkXWehmU = true;}
      if(ouajfmffOM == aBTCYMISII){SZrkJqqdFV = true;}
      else if(aBTCYMISII == ouajfmffOM){NmjlInOIxt = true;}
      if(hwDjEmiTtc == SnqCHJluOD){xgQsIruTSO = true;}
      else if(SnqCHJluOD == hwDjEmiTtc){kAmAYaRmuY = true;}
      if(tCQdFtbHYJ == dpZNNmHnrr){zJulcyIILC = true;}
      else if(dpZNNmHnrr == tCQdFtbHYJ){TmcxgJalnk = true;}
      if(nEsodkycIH == xqnHAchild){qzZrcCTnwG = true;}
      else if(xqnHAchild == nEsodkycIH){WppytyYdWT = true;}
      if(DBhZZdPYVN == KfDfYRKuOk){FxkRSULArg = true;}
      else if(KfDfYRKuOk == DBhZZdPYVN){qUWAhXZfLw = true;}
      if(STFgAmewel == tAOySjaFok){ulUFxJdGIL = true;}
      if(LXGEXcCNFT == EUaQLbzjCd){ihRCeyGYQF = true;}
      if(oFKmYrTpEb == aCIjsxltou){XHXLICXcSI = true;}
      while(tAOySjaFok == STFgAmewel){FrmOAaJlFy = true;}
      while(EUaQLbzjCd == EUaQLbzjCd){MbULXuKnFa = true;}
      while(aCIjsxltou == aCIjsxltou){CnQbhzPcOI = true;}
      if(cMYGxwbpnQ == true){cMYGxwbpnQ = false;}
      if(BctAYpFmTx == true){BctAYpFmTx = false;}
      if(SZrkJqqdFV == true){SZrkJqqdFV = false;}
      if(xgQsIruTSO == true){xgQsIruTSO = false;}
      if(zJulcyIILC == true){zJulcyIILC = false;}
      if(qzZrcCTnwG == true){qzZrcCTnwG = false;}
      if(FxkRSULArg == true){FxkRSULArg = false;}
      if(ulUFxJdGIL == true){ulUFxJdGIL = false;}
      if(ihRCeyGYQF == true){ihRCeyGYQF = false;}
      if(XHXLICXcSI == true){XHXLICXcSI = false;}
      if(lIQeHFdTuT == true){lIQeHFdTuT = false;}
      if(YNfkXWehmU == true){YNfkXWehmU = false;}
      if(NmjlInOIxt == true){NmjlInOIxt = false;}
      if(kAmAYaRmuY == true){kAmAYaRmuY = false;}
      if(TmcxgJalnk == true){TmcxgJalnk = false;}
      if(WppytyYdWT == true){WppytyYdWT = false;}
      if(qUWAhXZfLw == true){qUWAhXZfLw = false;}
      if(FrmOAaJlFy == true){FrmOAaJlFy = false;}
      if(MbULXuKnFa == true){MbULXuKnFa = false;}
      if(CnQbhzPcOI == true){CnQbhzPcOI = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FWCHWPZZQK
{ 
  void FBJVBxYjOE()
  { 
      bool lJERkuXSyf = false;
      bool wrQyGnGtnM = false;
      bool msnAMfRMBO = false;
      bool ZlQXZMhsQz = false;
      bool ilXcwjfLml = false;
      bool shLUEsUEEF = false;
      bool dLRjGKasYt = false;
      bool GMdndBUKDc = false;
      bool AFYPUrZwSf = false;
      bool qdoLVWTnWl = false;
      bool JlmsefEWqC = false;
      bool YJAbEXlCOO = false;
      bool jdZrCcXSSG = false;
      bool hgnRSNSfjn = false;
      bool mCEPHgcMGy = false;
      bool kRYIBeLtpC = false;
      bool IkEjUqViIq = false;
      bool FFRZagZKkU = false;
      bool AKMUVPHePn = false;
      bool CXphfByUtP = false;
      string NlbMjsgibx;
      string szOcyfhNXI;
      string NtHdFPZmXh;
      string OflFxgybhn;
      string QTTcYUCNqp;
      string SNuwyTexGU;
      string TtLshMNJeQ;
      string gGejqsaeFj;
      string MMugOBkpHd;
      string CBUNUWIZtL;
      string HshZJMiLEE;
      string sArWHGyddg;
      string bXBBOfkQPT;
      string kAjscTdVXC;
      string RYkCkCIyFN;
      string YduNiiqgKd;
      string aRwjTrPysn;
      string ccGbOCmzPM;
      string WlKBuGEWJs;
      string bwKtaAcAhr;
      if(NlbMjsgibx == HshZJMiLEE){lJERkuXSyf = true;}
      else if(HshZJMiLEE == NlbMjsgibx){JlmsefEWqC = true;}
      if(szOcyfhNXI == sArWHGyddg){wrQyGnGtnM = true;}
      else if(sArWHGyddg == szOcyfhNXI){YJAbEXlCOO = true;}
      if(NtHdFPZmXh == bXBBOfkQPT){msnAMfRMBO = true;}
      else if(bXBBOfkQPT == NtHdFPZmXh){jdZrCcXSSG = true;}
      if(OflFxgybhn == kAjscTdVXC){ZlQXZMhsQz = true;}
      else if(kAjscTdVXC == OflFxgybhn){hgnRSNSfjn = true;}
      if(QTTcYUCNqp == RYkCkCIyFN){ilXcwjfLml = true;}
      else if(RYkCkCIyFN == QTTcYUCNqp){mCEPHgcMGy = true;}
      if(SNuwyTexGU == YduNiiqgKd){shLUEsUEEF = true;}
      else if(YduNiiqgKd == SNuwyTexGU){kRYIBeLtpC = true;}
      if(TtLshMNJeQ == aRwjTrPysn){dLRjGKasYt = true;}
      else if(aRwjTrPysn == TtLshMNJeQ){IkEjUqViIq = true;}
      if(gGejqsaeFj == ccGbOCmzPM){GMdndBUKDc = true;}
      if(MMugOBkpHd == WlKBuGEWJs){AFYPUrZwSf = true;}
      if(CBUNUWIZtL == bwKtaAcAhr){qdoLVWTnWl = true;}
      while(ccGbOCmzPM == gGejqsaeFj){FFRZagZKkU = true;}
      while(WlKBuGEWJs == WlKBuGEWJs){AKMUVPHePn = true;}
      while(bwKtaAcAhr == bwKtaAcAhr){CXphfByUtP = true;}
      if(lJERkuXSyf == true){lJERkuXSyf = false;}
      if(wrQyGnGtnM == true){wrQyGnGtnM = false;}
      if(msnAMfRMBO == true){msnAMfRMBO = false;}
      if(ZlQXZMhsQz == true){ZlQXZMhsQz = false;}
      if(ilXcwjfLml == true){ilXcwjfLml = false;}
      if(shLUEsUEEF == true){shLUEsUEEF = false;}
      if(dLRjGKasYt == true){dLRjGKasYt = false;}
      if(GMdndBUKDc == true){GMdndBUKDc = false;}
      if(AFYPUrZwSf == true){AFYPUrZwSf = false;}
      if(qdoLVWTnWl == true){qdoLVWTnWl = false;}
      if(JlmsefEWqC == true){JlmsefEWqC = false;}
      if(YJAbEXlCOO == true){YJAbEXlCOO = false;}
      if(jdZrCcXSSG == true){jdZrCcXSSG = false;}
      if(hgnRSNSfjn == true){hgnRSNSfjn = false;}
      if(mCEPHgcMGy == true){mCEPHgcMGy = false;}
      if(kRYIBeLtpC == true){kRYIBeLtpC = false;}
      if(IkEjUqViIq == true){IkEjUqViIq = false;}
      if(FFRZagZKkU == true){FFRZagZKkU = false;}
      if(AKMUVPHePn == true){AKMUVPHePn = false;}
      if(CXphfByUtP == true){CXphfByUtP = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class DCFMHZWLNO
{ 
  void RCtuplZbVQ()
  { 
      bool dsnyUqoFdr = false;
      bool NpFiadCniH = false;
      bool SGuMuHeVLQ = false;
      bool WPVrFpMXFf = false;
      bool ImkZPpglBP = false;
      bool OiysZXxbCN = false;
      bool MStMVyUMdk = false;
      bool RehCyeUVwF = false;
      bool ZrUuCmRbSg = false;
      bool rkUrTpsXER = false;
      bool UTIPyiButt = false;
      bool CbexmTyyJh = false;
      bool frtwsgKpiI = false;
      bool lwRdkGjExS = false;
      bool lWhXeQFXEz = false;
      bool rtOVkbYLWR = false;
      bool YKLlHFJAMf = false;
      bool jtBpuJKWRb = false;
      bool alkjPqhurI = false;
      bool hXgftaKuPY = false;
      string VsEBzNsWAL;
      string yUasgYXnhy;
      string JFjQgxoRyk;
      string ZXYAjDLfwI;
      string FyHWssPkdL;
      string BNeOCPInTB;
      string YaXPlZwZIc;
      string MqqZsLyMWm;
      string gDnliMrKxU;
      string uXLtidZrkS;
      string xocAzKVFVq;
      string RwQgTUmlfY;
      string glcmhoewpO;
      string lFaDprammY;
      string pmnGDzGeZO;
      string qnMMlLMoRJ;
      string eYPGFLUNqb;
      string szyGhtxVji;
      string iAVaXysORd;
      string flUctRXgAr;
      if(VsEBzNsWAL == xocAzKVFVq){dsnyUqoFdr = true;}
      else if(xocAzKVFVq == VsEBzNsWAL){UTIPyiButt = true;}
      if(yUasgYXnhy == RwQgTUmlfY){NpFiadCniH = true;}
      else if(RwQgTUmlfY == yUasgYXnhy){CbexmTyyJh = true;}
      if(JFjQgxoRyk == glcmhoewpO){SGuMuHeVLQ = true;}
      else if(glcmhoewpO == JFjQgxoRyk){frtwsgKpiI = true;}
      if(ZXYAjDLfwI == lFaDprammY){WPVrFpMXFf = true;}
      else if(lFaDprammY == ZXYAjDLfwI){lwRdkGjExS = true;}
      if(FyHWssPkdL == pmnGDzGeZO){ImkZPpglBP = true;}
      else if(pmnGDzGeZO == FyHWssPkdL){lWhXeQFXEz = true;}
      if(BNeOCPInTB == qnMMlLMoRJ){OiysZXxbCN = true;}
      else if(qnMMlLMoRJ == BNeOCPInTB){rtOVkbYLWR = true;}
      if(YaXPlZwZIc == eYPGFLUNqb){MStMVyUMdk = true;}
      else if(eYPGFLUNqb == YaXPlZwZIc){YKLlHFJAMf = true;}
      if(MqqZsLyMWm == szyGhtxVji){RehCyeUVwF = true;}
      if(gDnliMrKxU == iAVaXysORd){ZrUuCmRbSg = true;}
      if(uXLtidZrkS == flUctRXgAr){rkUrTpsXER = true;}
      while(szyGhtxVji == MqqZsLyMWm){jtBpuJKWRb = true;}
      while(iAVaXysORd == iAVaXysORd){alkjPqhurI = true;}
      while(flUctRXgAr == flUctRXgAr){hXgftaKuPY = true;}
      if(dsnyUqoFdr == true){dsnyUqoFdr = false;}
      if(NpFiadCniH == true){NpFiadCniH = false;}
      if(SGuMuHeVLQ == true){SGuMuHeVLQ = false;}
      if(WPVrFpMXFf == true){WPVrFpMXFf = false;}
      if(ImkZPpglBP == true){ImkZPpglBP = false;}
      if(OiysZXxbCN == true){OiysZXxbCN = false;}
      if(MStMVyUMdk == true){MStMVyUMdk = false;}
      if(RehCyeUVwF == true){RehCyeUVwF = false;}
      if(ZrUuCmRbSg == true){ZrUuCmRbSg = false;}
      if(rkUrTpsXER == true){rkUrTpsXER = false;}
      if(UTIPyiButt == true){UTIPyiButt = false;}
      if(CbexmTyyJh == true){CbexmTyyJh = false;}
      if(frtwsgKpiI == true){frtwsgKpiI = false;}
      if(lwRdkGjExS == true){lwRdkGjExS = false;}
      if(lWhXeQFXEz == true){lWhXeQFXEz = false;}
      if(rtOVkbYLWR == true){rtOVkbYLWR = false;}
      if(YKLlHFJAMf == true){YKLlHFJAMf = false;}
      if(jtBpuJKWRb == true){jtBpuJKWRb = false;}
      if(alkjPqhurI == true){alkjPqhurI = false;}
      if(hXgftaKuPY == true){hXgftaKuPY = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SZRCWMSQAU
{ 
  void EGnqMaIyaC()
  { 
      bool deLqpYIgMg = false;
      bool OBKpZttCIS = false;
      bool JDbEsHfIOD = false;
      bool jPOMAxKbNX = false;
      bool ZtZgnRHiBw = false;
      bool KVHWPaDGxi = false;
      bool rzRJxzmxpc = false;
      bool jRojsPctQa = false;
      bool AxGSrOHrxZ = false;
      bool tDeefabQjo = false;
      bool eJsYLsgWcz = false;
      bool xIpVhurzQh = false;
      bool qtAQifmKER = false;
      bool mMJbwqDhGz = false;
      bool hjOxbkNIwk = false;
      bool hczBxjIcof = false;
      bool FVSLMjzOZq = false;
      bool qJrRAHfCny = false;
      bool SdNTjJNIdL = false;
      bool harasLRDkq = false;
      string wehsywLdCa;
      string DxqViDhMZD;
      string ekcFumBwmd;
      string yIrcRTqVAz;
      string uxYYXBOuiK;
      string RwBoJKBbyK;
      string tOpYctFNDP;
      string LLMZHgMDhs;
      string YGNcMCGDSH;
      string ROWoEfGzwc;
      string wArSyFjbgb;
      string xxtEknBNMa;
      string mBuuywoWUb;
      string aNHExYOtdG;
      string ReVqJTUSDQ;
      string erlUotFwUC;
      string QMGFBjppHM;
      string ZGjFIIfDld;
      string bVSEwhalOC;
      string TRZbpEXQoY;
      if(wehsywLdCa == wArSyFjbgb){deLqpYIgMg = true;}
      else if(wArSyFjbgb == wehsywLdCa){eJsYLsgWcz = true;}
      if(DxqViDhMZD == xxtEknBNMa){OBKpZttCIS = true;}
      else if(xxtEknBNMa == DxqViDhMZD){xIpVhurzQh = true;}
      if(ekcFumBwmd == mBuuywoWUb){JDbEsHfIOD = true;}
      else if(mBuuywoWUb == ekcFumBwmd){qtAQifmKER = true;}
      if(yIrcRTqVAz == aNHExYOtdG){jPOMAxKbNX = true;}
      else if(aNHExYOtdG == yIrcRTqVAz){mMJbwqDhGz = true;}
      if(uxYYXBOuiK == ReVqJTUSDQ){ZtZgnRHiBw = true;}
      else if(ReVqJTUSDQ == uxYYXBOuiK){hjOxbkNIwk = true;}
      if(RwBoJKBbyK == erlUotFwUC){KVHWPaDGxi = true;}
      else if(erlUotFwUC == RwBoJKBbyK){hczBxjIcof = true;}
      if(tOpYctFNDP == QMGFBjppHM){rzRJxzmxpc = true;}
      else if(QMGFBjppHM == tOpYctFNDP){FVSLMjzOZq = true;}
      if(LLMZHgMDhs == ZGjFIIfDld){jRojsPctQa = true;}
      if(YGNcMCGDSH == bVSEwhalOC){AxGSrOHrxZ = true;}
      if(ROWoEfGzwc == TRZbpEXQoY){tDeefabQjo = true;}
      while(ZGjFIIfDld == LLMZHgMDhs){qJrRAHfCny = true;}
      while(bVSEwhalOC == bVSEwhalOC){SdNTjJNIdL = true;}
      while(TRZbpEXQoY == TRZbpEXQoY){harasLRDkq = true;}
      if(deLqpYIgMg == true){deLqpYIgMg = false;}
      if(OBKpZttCIS == true){OBKpZttCIS = false;}
      if(JDbEsHfIOD == true){JDbEsHfIOD = false;}
      if(jPOMAxKbNX == true){jPOMAxKbNX = false;}
      if(ZtZgnRHiBw == true){ZtZgnRHiBw = false;}
      if(KVHWPaDGxi == true){KVHWPaDGxi = false;}
      if(rzRJxzmxpc == true){rzRJxzmxpc = false;}
      if(jRojsPctQa == true){jRojsPctQa = false;}
      if(AxGSrOHrxZ == true){AxGSrOHrxZ = false;}
      if(tDeefabQjo == true){tDeefabQjo = false;}
      if(eJsYLsgWcz == true){eJsYLsgWcz = false;}
      if(xIpVhurzQh == true){xIpVhurzQh = false;}
      if(qtAQifmKER == true){qtAQifmKER = false;}
      if(mMJbwqDhGz == true){mMJbwqDhGz = false;}
      if(hjOxbkNIwk == true){hjOxbkNIwk = false;}
      if(hczBxjIcof == true){hczBxjIcof = false;}
      if(FVSLMjzOZq == true){FVSLMjzOZq = false;}
      if(qJrRAHfCny == true){qJrRAHfCny = false;}
      if(SdNTjJNIdL == true){SdNTjJNIdL = false;}
      if(harasLRDkq == true){harasLRDkq = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SBUQVDCIQX
{ 
  void AwrbbDgSpZ()
  { 
      bool iTIRVVlcMc = false;
      bool RPbgrNSfZG = false;
      bool TcAZmjNVpW = false;
      bool kiQAgfLqfZ = false;
      bool VWhWrGeZJu = false;
      bool xemkBGShel = false;
      bool HQloriyazL = false;
      bool BbAAjxtdhg = false;
      bool FnLuZFwfza = false;
      bool NdMtREjseI = false;
      bool HEaxsUelBE = false;
      bool MnAxgTFqPl = false;
      bool lSaXxXPFie = false;
      bool MBzYYAMNBE = false;
      bool ZJdtOZdONy = false;
      bool zscrWocCzV = false;
      bool FZDVrLoaYf = false;
      bool GqoLLbnfJX = false;
      bool tBrkfSXGCi = false;
      bool XKNfzAGHCD = false;
      string guLLUpqKdX;
      string yOcfXdxmsL;
      string ApyshfdsjA;
      string kNtVeYNfVU;
      string LDuUBOuMTc;
      string NeVFfqxSco;
      string MxZcmjTGAy;
      string PQRXzxtcqT;
      string jQYHmGropm;
      string auCgIOcRgR;
      string toLUMwNyGi;
      string xTaJEuYSpg;
      string qeQNXTNkOk;
      string TbwZYuFEtq;
      string stiHpGUsbM;
      string pKbAZOYyxr;
      string hbiUPUXBjJ;
      string pUXznwwIWQ;
      string zUICuIAuAK;
      string VGGCIQLNrV;
      if(guLLUpqKdX == toLUMwNyGi){iTIRVVlcMc = true;}
      else if(toLUMwNyGi == guLLUpqKdX){HEaxsUelBE = true;}
      if(yOcfXdxmsL == xTaJEuYSpg){RPbgrNSfZG = true;}
      else if(xTaJEuYSpg == yOcfXdxmsL){MnAxgTFqPl = true;}
      if(ApyshfdsjA == qeQNXTNkOk){TcAZmjNVpW = true;}
      else if(qeQNXTNkOk == ApyshfdsjA){lSaXxXPFie = true;}
      if(kNtVeYNfVU == TbwZYuFEtq){kiQAgfLqfZ = true;}
      else if(TbwZYuFEtq == kNtVeYNfVU){MBzYYAMNBE = true;}
      if(LDuUBOuMTc == stiHpGUsbM){VWhWrGeZJu = true;}
      else if(stiHpGUsbM == LDuUBOuMTc){ZJdtOZdONy = true;}
      if(NeVFfqxSco == pKbAZOYyxr){xemkBGShel = true;}
      else if(pKbAZOYyxr == NeVFfqxSco){zscrWocCzV = true;}
      if(MxZcmjTGAy == hbiUPUXBjJ){HQloriyazL = true;}
      else if(hbiUPUXBjJ == MxZcmjTGAy){FZDVrLoaYf = true;}
      if(PQRXzxtcqT == pUXznwwIWQ){BbAAjxtdhg = true;}
      if(jQYHmGropm == zUICuIAuAK){FnLuZFwfza = true;}
      if(auCgIOcRgR == VGGCIQLNrV){NdMtREjseI = true;}
      while(pUXznwwIWQ == PQRXzxtcqT){GqoLLbnfJX = true;}
      while(zUICuIAuAK == zUICuIAuAK){tBrkfSXGCi = true;}
      while(VGGCIQLNrV == VGGCIQLNrV){XKNfzAGHCD = true;}
      if(iTIRVVlcMc == true){iTIRVVlcMc = false;}
      if(RPbgrNSfZG == true){RPbgrNSfZG = false;}
      if(TcAZmjNVpW == true){TcAZmjNVpW = false;}
      if(kiQAgfLqfZ == true){kiQAgfLqfZ = false;}
      if(VWhWrGeZJu == true){VWhWrGeZJu = false;}
      if(xemkBGShel == true){xemkBGShel = false;}
      if(HQloriyazL == true){HQloriyazL = false;}
      if(BbAAjxtdhg == true){BbAAjxtdhg = false;}
      if(FnLuZFwfza == true){FnLuZFwfza = false;}
      if(NdMtREjseI == true){NdMtREjseI = false;}
      if(HEaxsUelBE == true){HEaxsUelBE = false;}
      if(MnAxgTFqPl == true){MnAxgTFqPl = false;}
      if(lSaXxXPFie == true){lSaXxXPFie = false;}
      if(MBzYYAMNBE == true){MBzYYAMNBE = false;}
      if(ZJdtOZdONy == true){ZJdtOZdONy = false;}
      if(zscrWocCzV == true){zscrWocCzV = false;}
      if(FZDVrLoaYf == true){FZDVrLoaYf = false;}
      if(GqoLLbnfJX == true){GqoLLbnfJX = false;}
      if(tBrkfSXGCi == true){tBrkfSXGCi = false;}
      if(XKNfzAGHCD == true){XKNfzAGHCD = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class WTIMLSJMBP
{ 
  void DaLDbWlKas()
  { 
      bool wmuMSHICZd = false;
      bool HeFqFZroDp = false;
      bool jrcIReYadR = false;
      bool DNRDWDmPXx = false;
      bool hzryCtgRHV = false;
      bool RYJOlDJlum = false;
      bool hhXIZsNBTC = false;
      bool mcRFIjAeGX = false;
      bool yHTHzdECRS = false;
      bool tAUjOAaiXI = false;
      bool feyEhoGDGB = false;
      bool DdfWoaOQJw = false;
      bool oLjAImHhpI = false;
      bool JRHxqSJOYJ = false;
      bool XBuPPPTxtn = false;
      bool kKYbTTuxwu = false;
      bool hYMyrkWhfV = false;
      bool RAqAYnPTkr = false;
      bool SKmCWzrfdX = false;
      bool WLRdaynxWd = false;
      string giGwhPFJdT;
      string HgjcIidhZj;
      string PhZyQYAAXn;
      string hwNPWweGfh;
      string wfwuSuiUxi;
      string cCxpFzJIfJ;
      string OFkolKSUGM;
      string EWCrmikdeU;
      string pfDnKezXJW;
      string lVZWSqtGJH;
      string cNOjeFAFDV;
      string OecLSiBxeG;
      string LtKiZixfZu;
      string ndUXrbRSJE;
      string etUnIGrODi;
      string pxbCuhCfJw;
      string IUeIhkKFSq;
      string QhnZbRZWZS;
      string phbpEqBRbK;
      string YlIMwKOMFW;
      if(giGwhPFJdT == cNOjeFAFDV){wmuMSHICZd = true;}
      else if(cNOjeFAFDV == giGwhPFJdT){feyEhoGDGB = true;}
      if(HgjcIidhZj == OecLSiBxeG){HeFqFZroDp = true;}
      else if(OecLSiBxeG == HgjcIidhZj){DdfWoaOQJw = true;}
      if(PhZyQYAAXn == LtKiZixfZu){jrcIReYadR = true;}
      else if(LtKiZixfZu == PhZyQYAAXn){oLjAImHhpI = true;}
      if(hwNPWweGfh == ndUXrbRSJE){DNRDWDmPXx = true;}
      else if(ndUXrbRSJE == hwNPWweGfh){JRHxqSJOYJ = true;}
      if(wfwuSuiUxi == etUnIGrODi){hzryCtgRHV = true;}
      else if(etUnIGrODi == wfwuSuiUxi){XBuPPPTxtn = true;}
      if(cCxpFzJIfJ == pxbCuhCfJw){RYJOlDJlum = true;}
      else if(pxbCuhCfJw == cCxpFzJIfJ){kKYbTTuxwu = true;}
      if(OFkolKSUGM == IUeIhkKFSq){hhXIZsNBTC = true;}
      else if(IUeIhkKFSq == OFkolKSUGM){hYMyrkWhfV = true;}
      if(EWCrmikdeU == QhnZbRZWZS){mcRFIjAeGX = true;}
      if(pfDnKezXJW == phbpEqBRbK){yHTHzdECRS = true;}
      if(lVZWSqtGJH == YlIMwKOMFW){tAUjOAaiXI = true;}
      while(QhnZbRZWZS == EWCrmikdeU){RAqAYnPTkr = true;}
      while(phbpEqBRbK == phbpEqBRbK){SKmCWzrfdX = true;}
      while(YlIMwKOMFW == YlIMwKOMFW){WLRdaynxWd = true;}
      if(wmuMSHICZd == true){wmuMSHICZd = false;}
      if(HeFqFZroDp == true){HeFqFZroDp = false;}
      if(jrcIReYadR == true){jrcIReYadR = false;}
      if(DNRDWDmPXx == true){DNRDWDmPXx = false;}
      if(hzryCtgRHV == true){hzryCtgRHV = false;}
      if(RYJOlDJlum == true){RYJOlDJlum = false;}
      if(hhXIZsNBTC == true){hhXIZsNBTC = false;}
      if(mcRFIjAeGX == true){mcRFIjAeGX = false;}
      if(yHTHzdECRS == true){yHTHzdECRS = false;}
      if(tAUjOAaiXI == true){tAUjOAaiXI = false;}
      if(feyEhoGDGB == true){feyEhoGDGB = false;}
      if(DdfWoaOQJw == true){DdfWoaOQJw = false;}
      if(oLjAImHhpI == true){oLjAImHhpI = false;}
      if(JRHxqSJOYJ == true){JRHxqSJOYJ = false;}
      if(XBuPPPTxtn == true){XBuPPPTxtn = false;}
      if(kKYbTTuxwu == true){kKYbTTuxwu = false;}
      if(hYMyrkWhfV == true){hYMyrkWhfV = false;}
      if(RAqAYnPTkr == true){RAqAYnPTkr = false;}
      if(SKmCWzrfdX == true){SKmCWzrfdX = false;}
      if(WLRdaynxWd == true){WLRdaynxWd = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KSPLLKDMSG
{ 
  void VNKkqYWntE()
  { 
      bool xcfoyjZOrx = false;
      bool unLrJAtQHG = false;
      bool hKmjoMIjLC = false;
      bool WeBkPCLuSG = false;
      bool PjnYCRzawY = false;
      bool MtutQmpwAk = false;
      bool bpCoZPmacG = false;
      bool nyyApyVNhW = false;
      bool MgnmocjGTc = false;
      bool MPugeZhzzl = false;
      bool eYyokOwuDS = false;
      bool QcNdYtBOAx = false;
      bool ffGztXDQVQ = false;
      bool QWNuqmhyCa = false;
      bool fnCBLjCQHj = false;
      bool XSzCFzSyuM = false;
      bool KeZmCtORll = false;
      bool MDMYQrcEnK = false;
      bool VlIksxWLpr = false;
      bool UjDoPAOPzQ = false;
      string ETpxNSjUYa;
      string VLOUgrMOAO;
      string FYOPteNBdC;
      string pcXOqRZdMI;
      string wtXhCCqqVZ;
      string jCNTNfEZDh;
      string EhpVjsnAcO;
      string RiBqRKAfMq;
      string nWfESnPDAL;
      string MzaeEODCxJ;
      string OpNGHBBVCD;
      string eYMLsgBlCw;
      string cjzQBSHVmD;
      string GyhDcxIYwR;
      string ltBxfnlPkW;
      string MQzBfdqwFo;
      string sehywgqJiu;
      string YbpkJiiCjM;
      string frkQqpyfWy;
      string GLTGYHLDAz;
      if(ETpxNSjUYa == OpNGHBBVCD){xcfoyjZOrx = true;}
      else if(OpNGHBBVCD == ETpxNSjUYa){eYyokOwuDS = true;}
      if(VLOUgrMOAO == eYMLsgBlCw){unLrJAtQHG = true;}
      else if(eYMLsgBlCw == VLOUgrMOAO){QcNdYtBOAx = true;}
      if(FYOPteNBdC == cjzQBSHVmD){hKmjoMIjLC = true;}
      else if(cjzQBSHVmD == FYOPteNBdC){ffGztXDQVQ = true;}
      if(pcXOqRZdMI == GyhDcxIYwR){WeBkPCLuSG = true;}
      else if(GyhDcxIYwR == pcXOqRZdMI){QWNuqmhyCa = true;}
      if(wtXhCCqqVZ == ltBxfnlPkW){PjnYCRzawY = true;}
      else if(ltBxfnlPkW == wtXhCCqqVZ){fnCBLjCQHj = true;}
      if(jCNTNfEZDh == MQzBfdqwFo){MtutQmpwAk = true;}
      else if(MQzBfdqwFo == jCNTNfEZDh){XSzCFzSyuM = true;}
      if(EhpVjsnAcO == sehywgqJiu){bpCoZPmacG = true;}
      else if(sehywgqJiu == EhpVjsnAcO){KeZmCtORll = true;}
      if(RiBqRKAfMq == YbpkJiiCjM){nyyApyVNhW = true;}
      if(nWfESnPDAL == frkQqpyfWy){MgnmocjGTc = true;}
      if(MzaeEODCxJ == GLTGYHLDAz){MPugeZhzzl = true;}
      while(YbpkJiiCjM == RiBqRKAfMq){MDMYQrcEnK = true;}
      while(frkQqpyfWy == frkQqpyfWy){VlIksxWLpr = true;}
      while(GLTGYHLDAz == GLTGYHLDAz){UjDoPAOPzQ = true;}
      if(xcfoyjZOrx == true){xcfoyjZOrx = false;}
      if(unLrJAtQHG == true){unLrJAtQHG = false;}
      if(hKmjoMIjLC == true){hKmjoMIjLC = false;}
      if(WeBkPCLuSG == true){WeBkPCLuSG = false;}
      if(PjnYCRzawY == true){PjnYCRzawY = false;}
      if(MtutQmpwAk == true){MtutQmpwAk = false;}
      if(bpCoZPmacG == true){bpCoZPmacG = false;}
      if(nyyApyVNhW == true){nyyApyVNhW = false;}
      if(MgnmocjGTc == true){MgnmocjGTc = false;}
      if(MPugeZhzzl == true){MPugeZhzzl = false;}
      if(eYyokOwuDS == true){eYyokOwuDS = false;}
      if(QcNdYtBOAx == true){QcNdYtBOAx = false;}
      if(ffGztXDQVQ == true){ffGztXDQVQ = false;}
      if(QWNuqmhyCa == true){QWNuqmhyCa = false;}
      if(fnCBLjCQHj == true){fnCBLjCQHj = false;}
      if(XSzCFzSyuM == true){XSzCFzSyuM = false;}
      if(KeZmCtORll == true){KeZmCtORll = false;}
      if(MDMYQrcEnK == true){MDMYQrcEnK = false;}
      if(VlIksxWLpr == true){VlIksxWLpr = false;}
      if(UjDoPAOPzQ == true){UjDoPAOPzQ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NMRZDOFEJX
{ 
  void VDpHQtJrqI()
  { 
      bool hzQwFEdjIs = false;
      bool PYjtooJdww = false;
      bool WEnZPAWlmq = false;
      bool uRGioculQp = false;
      bool ZJTwSsoOKQ = false;
      bool BIJclcLpfE = false;
      bool hrwRyxRBFM = false;
      bool sWNFWNkrHW = false;
      bool LHaVomHdwL = false;
      bool RFGLIgMCxV = false;
      bool amIyWetEFX = false;
      bool rSiJrZQZbL = false;
      bool onEstnQBxo = false;
      bool BitkwXycyK = false;
      bool rDXYTyVxSh = false;
      bool QlZlfliPDi = false;
      bool OrHPWIYxNU = false;
      bool anbRSkPfZw = false;
      bool STiOYOIHFJ = false;
      bool oiZPuAwHBK = false;
      string SPdjSsFlPS;
      string QIEhmGzDnH;
      string IcnWblVOpU;
      string DDRFCNdKMe;
      string iRagJyHZfz;
      string auMwLobHHf;
      string WpwuOMpzeY;
      string bnqJYpCGMd;
      string dwhptHqUtM;
      string kWDfHrVeSw;
      string IItGXlEDXU;
      string mVtdCaHFBG;
      string VUERnFkLCf;
      string UWnQUUBbSH;
      string DnxYZPcYcf;
      string jQdLamDbSR;
      string BrBqAQgcIg;
      string ZfltChXerX;
      string cLhWuDsboh;
      string RYJAGLlFYJ;
      if(SPdjSsFlPS == IItGXlEDXU){hzQwFEdjIs = true;}
      else if(IItGXlEDXU == SPdjSsFlPS){amIyWetEFX = true;}
      if(QIEhmGzDnH == mVtdCaHFBG){PYjtooJdww = true;}
      else if(mVtdCaHFBG == QIEhmGzDnH){rSiJrZQZbL = true;}
      if(IcnWblVOpU == VUERnFkLCf){WEnZPAWlmq = true;}
      else if(VUERnFkLCf == IcnWblVOpU){onEstnQBxo = true;}
      if(DDRFCNdKMe == UWnQUUBbSH){uRGioculQp = true;}
      else if(UWnQUUBbSH == DDRFCNdKMe){BitkwXycyK = true;}
      if(iRagJyHZfz == DnxYZPcYcf){ZJTwSsoOKQ = true;}
      else if(DnxYZPcYcf == iRagJyHZfz){rDXYTyVxSh = true;}
      if(auMwLobHHf == jQdLamDbSR){BIJclcLpfE = true;}
      else if(jQdLamDbSR == auMwLobHHf){QlZlfliPDi = true;}
      if(WpwuOMpzeY == BrBqAQgcIg){hrwRyxRBFM = true;}
      else if(BrBqAQgcIg == WpwuOMpzeY){OrHPWIYxNU = true;}
      if(bnqJYpCGMd == ZfltChXerX){sWNFWNkrHW = true;}
      if(dwhptHqUtM == cLhWuDsboh){LHaVomHdwL = true;}
      if(kWDfHrVeSw == RYJAGLlFYJ){RFGLIgMCxV = true;}
      while(ZfltChXerX == bnqJYpCGMd){anbRSkPfZw = true;}
      while(cLhWuDsboh == cLhWuDsboh){STiOYOIHFJ = true;}
      while(RYJAGLlFYJ == RYJAGLlFYJ){oiZPuAwHBK = true;}
      if(hzQwFEdjIs == true){hzQwFEdjIs = false;}
      if(PYjtooJdww == true){PYjtooJdww = false;}
      if(WEnZPAWlmq == true){WEnZPAWlmq = false;}
      if(uRGioculQp == true){uRGioculQp = false;}
      if(ZJTwSsoOKQ == true){ZJTwSsoOKQ = false;}
      if(BIJclcLpfE == true){BIJclcLpfE = false;}
      if(hrwRyxRBFM == true){hrwRyxRBFM = false;}
      if(sWNFWNkrHW == true){sWNFWNkrHW = false;}
      if(LHaVomHdwL == true){LHaVomHdwL = false;}
      if(RFGLIgMCxV == true){RFGLIgMCxV = false;}
      if(amIyWetEFX == true){amIyWetEFX = false;}
      if(rSiJrZQZbL == true){rSiJrZQZbL = false;}
      if(onEstnQBxo == true){onEstnQBxo = false;}
      if(BitkwXycyK == true){BitkwXycyK = false;}
      if(rDXYTyVxSh == true){rDXYTyVxSh = false;}
      if(QlZlfliPDi == true){QlZlfliPDi = false;}
      if(OrHPWIYxNU == true){OrHPWIYxNU = false;}
      if(anbRSkPfZw == true){anbRSkPfZw = false;}
      if(STiOYOIHFJ == true){STiOYOIHFJ = false;}
      if(oiZPuAwHBK == true){oiZPuAwHBK = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SCCZBPJBQW
{ 
  void tWQhrTYitu()
  { 
      bool lHuXjYXGuS = false;
      bool JjuOTTYudb = false;
      bool JyAloIIwge = false;
      bool ypybIlgpQU = false;
      bool TPomhWtAgn = false;
      bool QFDGkmHIib = false;
      bool PbYPUSkTFg = false;
      bool lidJIsJMER = false;
      bool NIOrNslrqV = false;
      bool eXGoEqpZSO = false;
      bool katAChatxq = false;
      bool dtEQNHMgBa = false;
      bool frbjBOboEQ = false;
      bool CitpfmNNpf = false;
      bool RrtAqkTpax = false;
      bool dJXqhryJym = false;
      bool GlpgntzOja = false;
      bool sypVJnybrQ = false;
      bool ulNHlrLCIm = false;
      bool nEPCZeBNLx = false;
      string LbmopqlFcn;
      string LVVGcLuJzX;
      string TBaWztKsJd;
      string qmVEjGJRlW;
      string mbpLlMSluY;
      string asIAWKAPpw;
      string fswywYzRWW;
      string CcXbkrKPkl;
      string aYxkDydVgZ;
      string GsboqsfrOM;
      string MBPYVxUgPe;
      string zjeDUHwLuy;
      string udjxPIBhkq;
      string LSBXZYIXOF;
      string cwFCZVOYTi;
      string xUwQRhRSIN;
      string wmwXTfiGcw;
      string yXwpUyYhWt;
      string tbNBtqWjGq;
      string HpOAIpAhit;
      if(LbmopqlFcn == MBPYVxUgPe){lHuXjYXGuS = true;}
      else if(MBPYVxUgPe == LbmopqlFcn){katAChatxq = true;}
      if(LVVGcLuJzX == zjeDUHwLuy){JjuOTTYudb = true;}
      else if(zjeDUHwLuy == LVVGcLuJzX){dtEQNHMgBa = true;}
      if(TBaWztKsJd == udjxPIBhkq){JyAloIIwge = true;}
      else if(udjxPIBhkq == TBaWztKsJd){frbjBOboEQ = true;}
      if(qmVEjGJRlW == LSBXZYIXOF){ypybIlgpQU = true;}
      else if(LSBXZYIXOF == qmVEjGJRlW){CitpfmNNpf = true;}
      if(mbpLlMSluY == cwFCZVOYTi){TPomhWtAgn = true;}
      else if(cwFCZVOYTi == mbpLlMSluY){RrtAqkTpax = true;}
      if(asIAWKAPpw == xUwQRhRSIN){QFDGkmHIib = true;}
      else if(xUwQRhRSIN == asIAWKAPpw){dJXqhryJym = true;}
      if(fswywYzRWW == wmwXTfiGcw){PbYPUSkTFg = true;}
      else if(wmwXTfiGcw == fswywYzRWW){GlpgntzOja = true;}
      if(CcXbkrKPkl == yXwpUyYhWt){lidJIsJMER = true;}
      if(aYxkDydVgZ == tbNBtqWjGq){NIOrNslrqV = true;}
      if(GsboqsfrOM == HpOAIpAhit){eXGoEqpZSO = true;}
      while(yXwpUyYhWt == CcXbkrKPkl){sypVJnybrQ = true;}
      while(tbNBtqWjGq == tbNBtqWjGq){ulNHlrLCIm = true;}
      while(HpOAIpAhit == HpOAIpAhit){nEPCZeBNLx = true;}
      if(lHuXjYXGuS == true){lHuXjYXGuS = false;}
      if(JjuOTTYudb == true){JjuOTTYudb = false;}
      if(JyAloIIwge == true){JyAloIIwge = false;}
      if(ypybIlgpQU == true){ypybIlgpQU = false;}
      if(TPomhWtAgn == true){TPomhWtAgn = false;}
      if(QFDGkmHIib == true){QFDGkmHIib = false;}
      if(PbYPUSkTFg == true){PbYPUSkTFg = false;}
      if(lidJIsJMER == true){lidJIsJMER = false;}
      if(NIOrNslrqV == true){NIOrNslrqV = false;}
      if(eXGoEqpZSO == true){eXGoEqpZSO = false;}
      if(katAChatxq == true){katAChatxq = false;}
      if(dtEQNHMgBa == true){dtEQNHMgBa = false;}
      if(frbjBOboEQ == true){frbjBOboEQ = false;}
      if(CitpfmNNpf == true){CitpfmNNpf = false;}
      if(RrtAqkTpax == true){RrtAqkTpax = false;}
      if(dJXqhryJym == true){dJXqhryJym = false;}
      if(GlpgntzOja == true){GlpgntzOja = false;}
      if(sypVJnybrQ == true){sypVJnybrQ = false;}
      if(ulNHlrLCIm == true){ulNHlrLCIm = false;}
      if(nEPCZeBNLx == true){nEPCZeBNLx = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class YSBYLBOMMH
{ 
  void aTpVAFxZfR()
  { 
      bool eSOETyYQnz = false;
      bool CJlFSTDwKI = false;
      bool pXDDoZBeMt = false;
      bool FDfKglpLbj = false;
      bool EbIauDYakA = false;
      bool UBEKfGOLOt = false;
      bool wsEsszcaYg = false;
      bool drTZENPcIl = false;
      bool AatMZEyVea = false;
      bool WewhnKXOgw = false;
      bool ZyCDVcgeGR = false;
      bool pkDcsUzrSo = false;
      bool AHuyJyulls = false;
      bool pAwLMCYppx = false;
      bool WTPMjkbToS = false;
      bool LfuquRCEsX = false;
      bool gurMbooqQq = false;
      bool wJBVSgqiNX = false;
      bool WbxSgKFEQt = false;
      bool ATwSPbaCkt = false;
      string sgfCyJCtMc;
      string XxkkFqmAoD;
      string gEkyMkIqhu;
      string guDCTUkdtU;
      string YdoOYryXOf;
      string FxMlZnugpM;
      string AKMQubshRk;
      string GJFUdCOWZT;
      string mkcBpjiQce;
      string YEmgTORYSw;
      string yPGdZuqrIW;
      string ZYHWZPhKaL;
      string sbFlLBVXAP;
      string jpDcUWeybt;
      string rpIFFzxQHE;
      string gHxHCWJTRX;
      string ZTrsliUdBC;
      string kiigSDTzgL;
      string EFPuhcswZy;
      string NLgDUXxPTV;
      if(sgfCyJCtMc == yPGdZuqrIW){eSOETyYQnz = true;}
      else if(yPGdZuqrIW == sgfCyJCtMc){ZyCDVcgeGR = true;}
      if(XxkkFqmAoD == ZYHWZPhKaL){CJlFSTDwKI = true;}
      else if(ZYHWZPhKaL == XxkkFqmAoD){pkDcsUzrSo = true;}
      if(gEkyMkIqhu == sbFlLBVXAP){pXDDoZBeMt = true;}
      else if(sbFlLBVXAP == gEkyMkIqhu){AHuyJyulls = true;}
      if(guDCTUkdtU == jpDcUWeybt){FDfKglpLbj = true;}
      else if(jpDcUWeybt == guDCTUkdtU){pAwLMCYppx = true;}
      if(YdoOYryXOf == rpIFFzxQHE){EbIauDYakA = true;}
      else if(rpIFFzxQHE == YdoOYryXOf){WTPMjkbToS = true;}
      if(FxMlZnugpM == gHxHCWJTRX){UBEKfGOLOt = true;}
      else if(gHxHCWJTRX == FxMlZnugpM){LfuquRCEsX = true;}
      if(AKMQubshRk == ZTrsliUdBC){wsEsszcaYg = true;}
      else if(ZTrsliUdBC == AKMQubshRk){gurMbooqQq = true;}
      if(GJFUdCOWZT == kiigSDTzgL){drTZENPcIl = true;}
      if(mkcBpjiQce == EFPuhcswZy){AatMZEyVea = true;}
      if(YEmgTORYSw == NLgDUXxPTV){WewhnKXOgw = true;}
      while(kiigSDTzgL == GJFUdCOWZT){wJBVSgqiNX = true;}
      while(EFPuhcswZy == EFPuhcswZy){WbxSgKFEQt = true;}
      while(NLgDUXxPTV == NLgDUXxPTV){ATwSPbaCkt = true;}
      if(eSOETyYQnz == true){eSOETyYQnz = false;}
      if(CJlFSTDwKI == true){CJlFSTDwKI = false;}
      if(pXDDoZBeMt == true){pXDDoZBeMt = false;}
      if(FDfKglpLbj == true){FDfKglpLbj = false;}
      if(EbIauDYakA == true){EbIauDYakA = false;}
      if(UBEKfGOLOt == true){UBEKfGOLOt = false;}
      if(wsEsszcaYg == true){wsEsszcaYg = false;}
      if(drTZENPcIl == true){drTZENPcIl = false;}
      if(AatMZEyVea == true){AatMZEyVea = false;}
      if(WewhnKXOgw == true){WewhnKXOgw = false;}
      if(ZyCDVcgeGR == true){ZyCDVcgeGR = false;}
      if(pkDcsUzrSo == true){pkDcsUzrSo = false;}
      if(AHuyJyulls == true){AHuyJyulls = false;}
      if(pAwLMCYppx == true){pAwLMCYppx = false;}
      if(WTPMjkbToS == true){WTPMjkbToS = false;}
      if(LfuquRCEsX == true){LfuquRCEsX = false;}
      if(gurMbooqQq == true){gurMbooqQq = false;}
      if(wJBVSgqiNX == true){wJBVSgqiNX = false;}
      if(WbxSgKFEQt == true){WbxSgKFEQt = false;}
      if(ATwSPbaCkt == true){ATwSPbaCkt = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LSJVHMKQFV
{ 
  void ILjcbdDDPs()
  { 
      bool KOdpHjEESf = false;
      bool nuBCOkjtLa = false;
      bool cBupOaxlRi = false;
      bool gytWkRKbsQ = false;
      bool UXytsfpNmd = false;
      bool rUFoDJSmag = false;
      bool lRmjzpAeQw = false;
      bool aIBuFFCBxi = false;
      bool jjympLrETa = false;
      bool zWALdDkOtC = false;
      bool eLwpDjTJjx = false;
      bool HRprwBTzsm = false;
      bool xuKpxEHoRC = false;
      bool UnGtDYnHiG = false;
      bool rhUVTggIOb = false;
      bool jkUGFiAGsz = false;
      bool cdrhcSKuoX = false;
      bool NLQmuOaoVP = false;
      bool WzaoSjSAoT = false;
      bool XhOjdNplrA = false;
      string sFhSyAxuAW;
      string SEKoAAOAaL;
      string XFUSVKmgyY;
      string tbVysfYKtF;
      string objzcitrtn;
      string TKsgOnkOxn;
      string MPxbbqiLuU;
      string aDGUWwVUol;
      string PumKFjiniE;
      string tJNFsYeHWl;
      string zhwYMPDXJS;
      string SQZKXRblWr;
      string ItbpZQpAbx;
      string RNxDlhyOxo;
      string fLdQldjCga;
      string xrkecEGGyu;
      string YDuMAFSVXp;
      string SNngLxQKEw;
      string gnSAAZbRZh;
      string arPCPcPRQg;
      if(sFhSyAxuAW == zhwYMPDXJS){KOdpHjEESf = true;}
      else if(zhwYMPDXJS == sFhSyAxuAW){eLwpDjTJjx = true;}
      if(SEKoAAOAaL == SQZKXRblWr){nuBCOkjtLa = true;}
      else if(SQZKXRblWr == SEKoAAOAaL){HRprwBTzsm = true;}
      if(XFUSVKmgyY == ItbpZQpAbx){cBupOaxlRi = true;}
      else if(ItbpZQpAbx == XFUSVKmgyY){xuKpxEHoRC = true;}
      if(tbVysfYKtF == RNxDlhyOxo){gytWkRKbsQ = true;}
      else if(RNxDlhyOxo == tbVysfYKtF){UnGtDYnHiG = true;}
      if(objzcitrtn == fLdQldjCga){UXytsfpNmd = true;}
      else if(fLdQldjCga == objzcitrtn){rhUVTggIOb = true;}
      if(TKsgOnkOxn == xrkecEGGyu){rUFoDJSmag = true;}
      else if(xrkecEGGyu == TKsgOnkOxn){jkUGFiAGsz = true;}
      if(MPxbbqiLuU == YDuMAFSVXp){lRmjzpAeQw = true;}
      else if(YDuMAFSVXp == MPxbbqiLuU){cdrhcSKuoX = true;}
      if(aDGUWwVUol == SNngLxQKEw){aIBuFFCBxi = true;}
      if(PumKFjiniE == gnSAAZbRZh){jjympLrETa = true;}
      if(tJNFsYeHWl == arPCPcPRQg){zWALdDkOtC = true;}
      while(SNngLxQKEw == aDGUWwVUol){NLQmuOaoVP = true;}
      while(gnSAAZbRZh == gnSAAZbRZh){WzaoSjSAoT = true;}
      while(arPCPcPRQg == arPCPcPRQg){XhOjdNplrA = true;}
      if(KOdpHjEESf == true){KOdpHjEESf = false;}
      if(nuBCOkjtLa == true){nuBCOkjtLa = false;}
      if(cBupOaxlRi == true){cBupOaxlRi = false;}
      if(gytWkRKbsQ == true){gytWkRKbsQ = false;}
      if(UXytsfpNmd == true){UXytsfpNmd = false;}
      if(rUFoDJSmag == true){rUFoDJSmag = false;}
      if(lRmjzpAeQw == true){lRmjzpAeQw = false;}
      if(aIBuFFCBxi == true){aIBuFFCBxi = false;}
      if(jjympLrETa == true){jjympLrETa = false;}
      if(zWALdDkOtC == true){zWALdDkOtC = false;}
      if(eLwpDjTJjx == true){eLwpDjTJjx = false;}
      if(HRprwBTzsm == true){HRprwBTzsm = false;}
      if(xuKpxEHoRC == true){xuKpxEHoRC = false;}
      if(UnGtDYnHiG == true){UnGtDYnHiG = false;}
      if(rhUVTggIOb == true){rhUVTggIOb = false;}
      if(jkUGFiAGsz == true){jkUGFiAGsz = false;}
      if(cdrhcSKuoX == true){cdrhcSKuoX = false;}
      if(NLQmuOaoVP == true){NLQmuOaoVP = false;}
      if(WzaoSjSAoT == true){WzaoSjSAoT = false;}
      if(XhOjdNplrA == true){XhOjdNplrA = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class APDZBHKGTL
{ 
  void ssSpdzZUmj()
  { 
      bool SHwQkmCuuR = false;
      bool uLXGrZanBm = false;
      bool qzNWrhqxhc = false;
      bool DkDnEuUaDj = false;
      bool nVSRkVUEYq = false;
      bool wUOQpTsYYC = false;
      bool wFMSOKIiBx = false;
      bool EAkRyUfwkV = false;
      bool NlXpghVmyk = false;
      bool BwCAniJACa = false;
      bool NlCbmujxRV = false;
      bool nxmCoJgVlg = false;
      bool nUjQuIyjcP = false;
      bool LesKIbiuDk = false;
      bool xYVbxEIzLd = false;
      bool BmOVlrBlgd = false;
      bool xzTBJIDcsN = false;
      bool alSBiPzITp = false;
      bool AyYzOYPXYP = false;
      bool ZxOYmyETSn = false;
      string eUqhHokdwV;
      string chBcbAkGGz;
      string arFkeVgRpX;
      string qPMxASYkEg;
      string CdbMqKMkrc;
      string HCBjglPGDl;
      string ISmYdNUEbI;
      string azBYxxQrsU;
      string JuZNQoOcPN;
      string DShSYipDid;
      string oHIiCHAbmj;
      string VWNibMsZIn;
      string yrnOEHQhzN;
      string GMJOeBlpLI;
      string olpfqgFzqL;
      string pQutexTtet;
      string qRbYcJNyDZ;
      string aCOzzMBHWX;
      string CQqnXpROjg;
      string hrLQRWJQDh;
      if(eUqhHokdwV == oHIiCHAbmj){SHwQkmCuuR = true;}
      else if(oHIiCHAbmj == eUqhHokdwV){NlCbmujxRV = true;}
      if(chBcbAkGGz == VWNibMsZIn){uLXGrZanBm = true;}
      else if(VWNibMsZIn == chBcbAkGGz){nxmCoJgVlg = true;}
      if(arFkeVgRpX == yrnOEHQhzN){qzNWrhqxhc = true;}
      else if(yrnOEHQhzN == arFkeVgRpX){nUjQuIyjcP = true;}
      if(qPMxASYkEg == GMJOeBlpLI){DkDnEuUaDj = true;}
      else if(GMJOeBlpLI == qPMxASYkEg){LesKIbiuDk = true;}
      if(CdbMqKMkrc == olpfqgFzqL){nVSRkVUEYq = true;}
      else if(olpfqgFzqL == CdbMqKMkrc){xYVbxEIzLd = true;}
      if(HCBjglPGDl == pQutexTtet){wUOQpTsYYC = true;}
      else if(pQutexTtet == HCBjglPGDl){BmOVlrBlgd = true;}
      if(ISmYdNUEbI == qRbYcJNyDZ){wFMSOKIiBx = true;}
      else if(qRbYcJNyDZ == ISmYdNUEbI){xzTBJIDcsN = true;}
      if(azBYxxQrsU == aCOzzMBHWX){EAkRyUfwkV = true;}
      if(JuZNQoOcPN == CQqnXpROjg){NlXpghVmyk = true;}
      if(DShSYipDid == hrLQRWJQDh){BwCAniJACa = true;}
      while(aCOzzMBHWX == azBYxxQrsU){alSBiPzITp = true;}
      while(CQqnXpROjg == CQqnXpROjg){AyYzOYPXYP = true;}
      while(hrLQRWJQDh == hrLQRWJQDh){ZxOYmyETSn = true;}
      if(SHwQkmCuuR == true){SHwQkmCuuR = false;}
      if(uLXGrZanBm == true){uLXGrZanBm = false;}
      if(qzNWrhqxhc == true){qzNWrhqxhc = false;}
      if(DkDnEuUaDj == true){DkDnEuUaDj = false;}
      if(nVSRkVUEYq == true){nVSRkVUEYq = false;}
      if(wUOQpTsYYC == true){wUOQpTsYYC = false;}
      if(wFMSOKIiBx == true){wFMSOKIiBx = false;}
      if(EAkRyUfwkV == true){EAkRyUfwkV = false;}
      if(NlXpghVmyk == true){NlXpghVmyk = false;}
      if(BwCAniJACa == true){BwCAniJACa = false;}
      if(NlCbmujxRV == true){NlCbmujxRV = false;}
      if(nxmCoJgVlg == true){nxmCoJgVlg = false;}
      if(nUjQuIyjcP == true){nUjQuIyjcP = false;}
      if(LesKIbiuDk == true){LesKIbiuDk = false;}
      if(xYVbxEIzLd == true){xYVbxEIzLd = false;}
      if(BmOVlrBlgd == true){BmOVlrBlgd = false;}
      if(xzTBJIDcsN == true){xzTBJIDcsN = false;}
      if(alSBiPzITp == true){alSBiPzITp = false;}
      if(AyYzOYPXYP == true){AyYzOYPXYP = false;}
      if(ZxOYmyETSn == true){ZxOYmyETSn = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class GEWPJGATLE
{ 
  void pMPZFkZttr()
  { 
      bool abqbYIOAqN = false;
      bool OzrxuNsmjQ = false;
      bool KFNgcglFJI = false;
      bool UGaEcscIfl = false;
      bool LnoRKgNHLt = false;
      bool PIlNrUOKyr = false;
      bool WIqirIYnUB = false;
      bool jjHxEWPuVw = false;
      bool SdtPrLUNLH = false;
      bool dSJmPDxGZC = false;
      bool HZmJJeBnIV = false;
      bool ESnCZsNapf = false;
      bool mLbMWQaUhB = false;
      bool ZJVSnrqkFN = false;
      bool KgEpCKdHNV = false;
      bool oVPsPwyZlr = false;
      bool EgFHwuxVNg = false;
      bool CcUcVGGkJM = false;
      bool mSGLMDtbDq = false;
      bool tQyNIcaxtH = false;
      string miphDBgPak;
      string nIEPeqcEnW;
      string izEoSnrFCf;
      string thBQwValyP;
      string aXTCrdUcdX;
      string pGiXDmeBOP;
      string JaNupGHnTJ;
      string QpFXwLfBxo;
      string SPbdomzcNJ;
      string qZXiGzJYLl;
      string WfJTHAAKbr;
      string GcEZwsziGD;
      string GdaJRqSWRw;
      string BHpKSlGHWo;
      string loBUCNVylM;
      string AaRIerFzBQ;
      string hdrfqzmsGK;
      string mqMQQRcFHD;
      string VTkEuZogfZ;
      string gGFGHIwVdC;
      if(miphDBgPak == WfJTHAAKbr){abqbYIOAqN = true;}
      else if(WfJTHAAKbr == miphDBgPak){HZmJJeBnIV = true;}
      if(nIEPeqcEnW == GcEZwsziGD){OzrxuNsmjQ = true;}
      else if(GcEZwsziGD == nIEPeqcEnW){ESnCZsNapf = true;}
      if(izEoSnrFCf == GdaJRqSWRw){KFNgcglFJI = true;}
      else if(GdaJRqSWRw == izEoSnrFCf){mLbMWQaUhB = true;}
      if(thBQwValyP == BHpKSlGHWo){UGaEcscIfl = true;}
      else if(BHpKSlGHWo == thBQwValyP){ZJVSnrqkFN = true;}
      if(aXTCrdUcdX == loBUCNVylM){LnoRKgNHLt = true;}
      else if(loBUCNVylM == aXTCrdUcdX){KgEpCKdHNV = true;}
      if(pGiXDmeBOP == AaRIerFzBQ){PIlNrUOKyr = true;}
      else if(AaRIerFzBQ == pGiXDmeBOP){oVPsPwyZlr = true;}
      if(JaNupGHnTJ == hdrfqzmsGK){WIqirIYnUB = true;}
      else if(hdrfqzmsGK == JaNupGHnTJ){EgFHwuxVNg = true;}
      if(QpFXwLfBxo == mqMQQRcFHD){jjHxEWPuVw = true;}
      if(SPbdomzcNJ == VTkEuZogfZ){SdtPrLUNLH = true;}
      if(qZXiGzJYLl == gGFGHIwVdC){dSJmPDxGZC = true;}
      while(mqMQQRcFHD == QpFXwLfBxo){CcUcVGGkJM = true;}
      while(VTkEuZogfZ == VTkEuZogfZ){mSGLMDtbDq = true;}
      while(gGFGHIwVdC == gGFGHIwVdC){tQyNIcaxtH = true;}
      if(abqbYIOAqN == true){abqbYIOAqN = false;}
      if(OzrxuNsmjQ == true){OzrxuNsmjQ = false;}
      if(KFNgcglFJI == true){KFNgcglFJI = false;}
      if(UGaEcscIfl == true){UGaEcscIfl = false;}
      if(LnoRKgNHLt == true){LnoRKgNHLt = false;}
      if(PIlNrUOKyr == true){PIlNrUOKyr = false;}
      if(WIqirIYnUB == true){WIqirIYnUB = false;}
      if(jjHxEWPuVw == true){jjHxEWPuVw = false;}
      if(SdtPrLUNLH == true){SdtPrLUNLH = false;}
      if(dSJmPDxGZC == true){dSJmPDxGZC = false;}
      if(HZmJJeBnIV == true){HZmJJeBnIV = false;}
      if(ESnCZsNapf == true){ESnCZsNapf = false;}
      if(mLbMWQaUhB == true){mLbMWQaUhB = false;}
      if(ZJVSnrqkFN == true){ZJVSnrqkFN = false;}
      if(KgEpCKdHNV == true){KgEpCKdHNV = false;}
      if(oVPsPwyZlr == true){oVPsPwyZlr = false;}
      if(EgFHwuxVNg == true){EgFHwuxVNg = false;}
      if(CcUcVGGkJM == true){CcUcVGGkJM = false;}
      if(mSGLMDtbDq == true){mSGLMDtbDq = false;}
      if(tQyNIcaxtH == true){tQyNIcaxtH = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class LULODIOBIH
{ 
  void SOQtYjaZQY()
  { 
      bool jBzXXZYrVm = false;
      bool WlOiDKVzxy = false;
      bool kgpZnLNlQl = false;
      bool suofyiMRwG = false;
      bool AiTlMnAWlS = false;
      bool gfkMhCKZjL = false;
      bool MmaisbwhjT = false;
      bool xDDqyBNEMA = false;
      bool LqzxFNJcyu = false;
      bool XlhjtMHjrk = false;
      bool qRmSUdctOg = false;
      bool oGMjAUnddf = false;
      bool RTjnuKVSwk = false;
      bool scLbIsEfba = false;
      bool kONQGRZyjY = false;
      bool YdFLSXhyXJ = false;
      bool HJxcnLtkbG = false;
      bool QHBCuDFShd = false;
      bool kXhXLOXAIj = false;
      bool LFIRTaQkct = false;
      string pwupegnFyq;
      string JlWQFMsKET;
      string LSzVWVEPfK;
      string gxfHeerUVg;
      string IfVcyxlfiK;
      string QcKObXwZSJ;
      string cXZnKLGPlZ;
      string UXnWKewVXZ;
      string iXgUsEmQeG;
      string iSCLEkFhSl;
      string pWtJprnmEw;
      string aaBsZFqFuG;
      string YpjeqCVbSZ;
      string YOCliMdXdA;
      string PzMifHQJlz;
      string sAmMMAmORn;
      string WMTdWMXPJN;
      string aTbtpYdBzi;
      string kMpTfYbpYi;
      string ZPFpdDfiLS;
      if(pwupegnFyq == pWtJprnmEw){jBzXXZYrVm = true;}
      else if(pWtJprnmEw == pwupegnFyq){qRmSUdctOg = true;}
      if(JlWQFMsKET == aaBsZFqFuG){WlOiDKVzxy = true;}
      else if(aaBsZFqFuG == JlWQFMsKET){oGMjAUnddf = true;}
      if(LSzVWVEPfK == YpjeqCVbSZ){kgpZnLNlQl = true;}
      else if(YpjeqCVbSZ == LSzVWVEPfK){RTjnuKVSwk = true;}
      if(gxfHeerUVg == YOCliMdXdA){suofyiMRwG = true;}
      else if(YOCliMdXdA == gxfHeerUVg){scLbIsEfba = true;}
      if(IfVcyxlfiK == PzMifHQJlz){AiTlMnAWlS = true;}
      else if(PzMifHQJlz == IfVcyxlfiK){kONQGRZyjY = true;}
      if(QcKObXwZSJ == sAmMMAmORn){gfkMhCKZjL = true;}
      else if(sAmMMAmORn == QcKObXwZSJ){YdFLSXhyXJ = true;}
      if(cXZnKLGPlZ == WMTdWMXPJN){MmaisbwhjT = true;}
      else if(WMTdWMXPJN == cXZnKLGPlZ){HJxcnLtkbG = true;}
      if(UXnWKewVXZ == aTbtpYdBzi){xDDqyBNEMA = true;}
      if(iXgUsEmQeG == kMpTfYbpYi){LqzxFNJcyu = true;}
      if(iSCLEkFhSl == ZPFpdDfiLS){XlhjtMHjrk = true;}
      while(aTbtpYdBzi == UXnWKewVXZ){QHBCuDFShd = true;}
      while(kMpTfYbpYi == kMpTfYbpYi){kXhXLOXAIj = true;}
      while(ZPFpdDfiLS == ZPFpdDfiLS){LFIRTaQkct = true;}
      if(jBzXXZYrVm == true){jBzXXZYrVm = false;}
      if(WlOiDKVzxy == true){WlOiDKVzxy = false;}
      if(kgpZnLNlQl == true){kgpZnLNlQl = false;}
      if(suofyiMRwG == true){suofyiMRwG = false;}
      if(AiTlMnAWlS == true){AiTlMnAWlS = false;}
      if(gfkMhCKZjL == true){gfkMhCKZjL = false;}
      if(MmaisbwhjT == true){MmaisbwhjT = false;}
      if(xDDqyBNEMA == true){xDDqyBNEMA = false;}
      if(LqzxFNJcyu == true){LqzxFNJcyu = false;}
      if(XlhjtMHjrk == true){XlhjtMHjrk = false;}
      if(qRmSUdctOg == true){qRmSUdctOg = false;}
      if(oGMjAUnddf == true){oGMjAUnddf = false;}
      if(RTjnuKVSwk == true){RTjnuKVSwk = false;}
      if(scLbIsEfba == true){scLbIsEfba = false;}
      if(kONQGRZyjY == true){kONQGRZyjY = false;}
      if(YdFLSXhyXJ == true){YdFLSXhyXJ = false;}
      if(HJxcnLtkbG == true){HJxcnLtkbG = false;}
      if(QHBCuDFShd == true){QHBCuDFShd = false;}
      if(kXhXLOXAIj == true){kXhXLOXAIj = false;}
      if(LFIRTaQkct == true){LFIRTaQkct = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QLPASVPQRU
{ 
  void ILAegzWVSW()
  { 
      bool KINbDgBZcd = false;
      bool HTTAtHQHMf = false;
      bool okyJjfUUTV = false;
      bool uIuHEEfMPe = false;
      bool aJDFFJAqkk = false;
      bool RNtscYKEqW = false;
      bool LgFGhyXLwp = false;
      bool bEqgEfgsDa = false;
      bool fdjhDhOCee = false;
      bool taUrStDwWH = false;
      bool koutJNVMnw = false;
      bool qQAPqhwXDA = false;
      bool iVfQwBizNJ = false;
      bool bjVwpqhoYp = false;
      bool LBNhmcZFAk = false;
      bool xesWXPIdyI = false;
      bool dVzeUGuEZA = false;
      bool zHzMnhHxdT = false;
      bool adzcLxqzYZ = false;
      bool hEqPMoCiNf = false;
      string CRrnOMQnKQ;
      string gaHqtxVDme;
      string prnwdaccGb;
      string AcSgrouAGz;
      string FVctLaSHMK;
      string AdjGJJcuJm;
      string imhiQEHayE;
      string CUwmZTPtkD;
      string IVQuaVrMXi;
      string MAqTQRntdG;
      string SqIHIfhwgo;
      string siKGdMWlHt;
      string MuDPrBdcWH;
      string fVQdjQMIlu;
      string nufhocojoD;
      string uClitWnsPG;
      string BEZYJXSgVN;
      string ANAMFQPUZO;
      string MsnSMcigiM;
      string cTresiUGYR;
      if(CRrnOMQnKQ == SqIHIfhwgo){KINbDgBZcd = true;}
      else if(SqIHIfhwgo == CRrnOMQnKQ){koutJNVMnw = true;}
      if(gaHqtxVDme == siKGdMWlHt){HTTAtHQHMf = true;}
      else if(siKGdMWlHt == gaHqtxVDme){qQAPqhwXDA = true;}
      if(prnwdaccGb == MuDPrBdcWH){okyJjfUUTV = true;}
      else if(MuDPrBdcWH == prnwdaccGb){iVfQwBizNJ = true;}
      if(AcSgrouAGz == fVQdjQMIlu){uIuHEEfMPe = true;}
      else if(fVQdjQMIlu == AcSgrouAGz){bjVwpqhoYp = true;}
      if(FVctLaSHMK == nufhocojoD){aJDFFJAqkk = true;}
      else if(nufhocojoD == FVctLaSHMK){LBNhmcZFAk = true;}
      if(AdjGJJcuJm == uClitWnsPG){RNtscYKEqW = true;}
      else if(uClitWnsPG == AdjGJJcuJm){xesWXPIdyI = true;}
      if(imhiQEHayE == BEZYJXSgVN){LgFGhyXLwp = true;}
      else if(BEZYJXSgVN == imhiQEHayE){dVzeUGuEZA = true;}
      if(CUwmZTPtkD == ANAMFQPUZO){bEqgEfgsDa = true;}
      if(IVQuaVrMXi == MsnSMcigiM){fdjhDhOCee = true;}
      if(MAqTQRntdG == cTresiUGYR){taUrStDwWH = true;}
      while(ANAMFQPUZO == CUwmZTPtkD){zHzMnhHxdT = true;}
      while(MsnSMcigiM == MsnSMcigiM){adzcLxqzYZ = true;}
      while(cTresiUGYR == cTresiUGYR){hEqPMoCiNf = true;}
      if(KINbDgBZcd == true){KINbDgBZcd = false;}
      if(HTTAtHQHMf == true){HTTAtHQHMf = false;}
      if(okyJjfUUTV == true){okyJjfUUTV = false;}
      if(uIuHEEfMPe == true){uIuHEEfMPe = false;}
      if(aJDFFJAqkk == true){aJDFFJAqkk = false;}
      if(RNtscYKEqW == true){RNtscYKEqW = false;}
      if(LgFGhyXLwp == true){LgFGhyXLwp = false;}
      if(bEqgEfgsDa == true){bEqgEfgsDa = false;}
      if(fdjhDhOCee == true){fdjhDhOCee = false;}
      if(taUrStDwWH == true){taUrStDwWH = false;}
      if(koutJNVMnw == true){koutJNVMnw = false;}
      if(qQAPqhwXDA == true){qQAPqhwXDA = false;}
      if(iVfQwBizNJ == true){iVfQwBizNJ = false;}
      if(bjVwpqhoYp == true){bjVwpqhoYp = false;}
      if(LBNhmcZFAk == true){LBNhmcZFAk = false;}
      if(xesWXPIdyI == true){xesWXPIdyI = false;}
      if(dVzeUGuEZA == true){dVzeUGuEZA = false;}
      if(zHzMnhHxdT == true){zHzMnhHxdT = false;}
      if(adzcLxqzYZ == true){adzcLxqzYZ = false;}
      if(hEqPMoCiNf == true){hEqPMoCiNf = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class WZWPLDGRXZ
{ 
  void MIuanwguIM()
  { 
      bool HfyqwHVSgf = false;
      bool ZUSPSQyqsK = false;
      bool CWnYwnVCfc = false;
      bool RSHMhrKMCS = false;
      bool pfWBdWSiDb = false;
      bool ZBXPajqYtH = false;
      bool kSlDOGNFga = false;
      bool FloaTVAmtZ = false;
      bool VmzEDQuqlx = false;
      bool BkRuhXtTmX = false;
      bool IMBcVnLxwE = false;
      bool jEdUlyThzy = false;
      bool LVTjYTTtsw = false;
      bool bfAWQIkViO = false;
      bool ExuhNwHxIj = false;
      bool EMToMmsRDB = false;
      bool TREAbDEHDc = false;
      bool MxzksNZkVb = false;
      bool MWkJqPLKJd = false;
      bool OTRgulruFJ = false;
      string yaautrttLA;
      string JPIkiOJkBR;
      string zeKbHbxjWq;
      string FUkyqMsGim;
      string LHEJWMuXBn;
      string NoZbVTPymR;
      string tJWUtneJsV;
      string VDBNFqaBFu;
      string gYihcPuIcX;
      string xtSQrCjiOh;
      string gkdeSKmLWd;
      string rOrqkVZoqe;
      string sWgUYajjVC;
      string HqmWOwuEOS;
      string bikyeiBafW;
      string cuqioKEIFr;
      string ZDpTWBaVco;
      string ZUZYCJkipp;
      string YIEyZLzDgD;
      string rlyWrYrRac;
      if(yaautrttLA == gkdeSKmLWd){HfyqwHVSgf = true;}
      else if(gkdeSKmLWd == yaautrttLA){IMBcVnLxwE = true;}
      if(JPIkiOJkBR == rOrqkVZoqe){ZUSPSQyqsK = true;}
      else if(rOrqkVZoqe == JPIkiOJkBR){jEdUlyThzy = true;}
      if(zeKbHbxjWq == sWgUYajjVC){CWnYwnVCfc = true;}
      else if(sWgUYajjVC == zeKbHbxjWq){LVTjYTTtsw = true;}
      if(FUkyqMsGim == HqmWOwuEOS){RSHMhrKMCS = true;}
      else if(HqmWOwuEOS == FUkyqMsGim){bfAWQIkViO = true;}
      if(LHEJWMuXBn == bikyeiBafW){pfWBdWSiDb = true;}
      else if(bikyeiBafW == LHEJWMuXBn){ExuhNwHxIj = true;}
      if(NoZbVTPymR == cuqioKEIFr){ZBXPajqYtH = true;}
      else if(cuqioKEIFr == NoZbVTPymR){EMToMmsRDB = true;}
      if(tJWUtneJsV == ZDpTWBaVco){kSlDOGNFga = true;}
      else if(ZDpTWBaVco == tJWUtneJsV){TREAbDEHDc = true;}
      if(VDBNFqaBFu == ZUZYCJkipp){FloaTVAmtZ = true;}
      if(gYihcPuIcX == YIEyZLzDgD){VmzEDQuqlx = true;}
      if(xtSQrCjiOh == rlyWrYrRac){BkRuhXtTmX = true;}
      while(ZUZYCJkipp == VDBNFqaBFu){MxzksNZkVb = true;}
      while(YIEyZLzDgD == YIEyZLzDgD){MWkJqPLKJd = true;}
      while(rlyWrYrRac == rlyWrYrRac){OTRgulruFJ = true;}
      if(HfyqwHVSgf == true){HfyqwHVSgf = false;}
      if(ZUSPSQyqsK == true){ZUSPSQyqsK = false;}
      if(CWnYwnVCfc == true){CWnYwnVCfc = false;}
      if(RSHMhrKMCS == true){RSHMhrKMCS = false;}
      if(pfWBdWSiDb == true){pfWBdWSiDb = false;}
      if(ZBXPajqYtH == true){ZBXPajqYtH = false;}
      if(kSlDOGNFga == true){kSlDOGNFga = false;}
      if(FloaTVAmtZ == true){FloaTVAmtZ = false;}
      if(VmzEDQuqlx == true){VmzEDQuqlx = false;}
      if(BkRuhXtTmX == true){BkRuhXtTmX = false;}
      if(IMBcVnLxwE == true){IMBcVnLxwE = false;}
      if(jEdUlyThzy == true){jEdUlyThzy = false;}
      if(LVTjYTTtsw == true){LVTjYTTtsw = false;}
      if(bfAWQIkViO == true){bfAWQIkViO = false;}
      if(ExuhNwHxIj == true){ExuhNwHxIj = false;}
      if(EMToMmsRDB == true){EMToMmsRDB = false;}
      if(TREAbDEHDc == true){TREAbDEHDc = false;}
      if(MxzksNZkVb == true){MxzksNZkVb = false;}
      if(MWkJqPLKJd == true){MWkJqPLKJd = false;}
      if(OTRgulruFJ == true){OTRgulruFJ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class UEZWBTCARG
{ 
  void JDjZQOMmzH()
  { 
      bool UJDPXtXdyd = false;
      bool sqyEuOPOpD = false;
      bool rZNZcwWOTc = false;
      bool GThCCouFWb = false;
      bool hWpFzXVfEd = false;
      bool SDwnCuwdla = false;
      bool XJJUWYJuCG = false;
      bool TKrNUViHZF = false;
      bool ZpJpKCXtAH = false;
      bool sQTuEYMUhZ = false;
      bool NZdxJILhSg = false;
      bool LnDfAgzTKU = false;
      bool BPutNDpwJw = false;
      bool INYzqzMLfC = false;
      bool qoaTuENmtX = false;
      bool ZrKNYyeijG = false;
      bool ROYWXxOEoB = false;
      bool UmnIdLnbzJ = false;
      bool pDCkWPDLas = false;
      bool UcbYjbQEuy = false;
      string csjToYorqa;
      string gIZntIFqAc;
      string sgsLDquame;
      string LNPxXtNAex;
      string iCZFWBTxgs;
      string obtetuAXPx;
      string dNELPTscPA;
      string tFLCsqHnoC;
      string XxstpjSkDc;
      string tojhTVxNoD;
      string IqhgziPqkC;
      string nLiNZfTKUf;
      string AHZlFEUdhX;
      string ssyltgPMuH;
      string TbAAohGWIn;
      string BmrJCAQubt;
      string NkIMItNxSs;
      string QHPNLYHYxW;
      string yyVtWFBnis;
      string PTkAshSmqu;
      if(csjToYorqa == IqhgziPqkC){UJDPXtXdyd = true;}
      else if(IqhgziPqkC == csjToYorqa){NZdxJILhSg = true;}
      if(gIZntIFqAc == nLiNZfTKUf){sqyEuOPOpD = true;}
      else if(nLiNZfTKUf == gIZntIFqAc){LnDfAgzTKU = true;}
      if(sgsLDquame == AHZlFEUdhX){rZNZcwWOTc = true;}
      else if(AHZlFEUdhX == sgsLDquame){BPutNDpwJw = true;}
      if(LNPxXtNAex == ssyltgPMuH){GThCCouFWb = true;}
      else if(ssyltgPMuH == LNPxXtNAex){INYzqzMLfC = true;}
      if(iCZFWBTxgs == TbAAohGWIn){hWpFzXVfEd = true;}
      else if(TbAAohGWIn == iCZFWBTxgs){qoaTuENmtX = true;}
      if(obtetuAXPx == BmrJCAQubt){SDwnCuwdla = true;}
      else if(BmrJCAQubt == obtetuAXPx){ZrKNYyeijG = true;}
      if(dNELPTscPA == NkIMItNxSs){XJJUWYJuCG = true;}
      else if(NkIMItNxSs == dNELPTscPA){ROYWXxOEoB = true;}
      if(tFLCsqHnoC == QHPNLYHYxW){TKrNUViHZF = true;}
      if(XxstpjSkDc == yyVtWFBnis){ZpJpKCXtAH = true;}
      if(tojhTVxNoD == PTkAshSmqu){sQTuEYMUhZ = true;}
      while(QHPNLYHYxW == tFLCsqHnoC){UmnIdLnbzJ = true;}
      while(yyVtWFBnis == yyVtWFBnis){pDCkWPDLas = true;}
      while(PTkAshSmqu == PTkAshSmqu){UcbYjbQEuy = true;}
      if(UJDPXtXdyd == true){UJDPXtXdyd = false;}
      if(sqyEuOPOpD == true){sqyEuOPOpD = false;}
      if(rZNZcwWOTc == true){rZNZcwWOTc = false;}
      if(GThCCouFWb == true){GThCCouFWb = false;}
      if(hWpFzXVfEd == true){hWpFzXVfEd = false;}
      if(SDwnCuwdla == true){SDwnCuwdla = false;}
      if(XJJUWYJuCG == true){XJJUWYJuCG = false;}
      if(TKrNUViHZF == true){TKrNUViHZF = false;}
      if(ZpJpKCXtAH == true){ZpJpKCXtAH = false;}
      if(sQTuEYMUhZ == true){sQTuEYMUhZ = false;}
      if(NZdxJILhSg == true){NZdxJILhSg = false;}
      if(LnDfAgzTKU == true){LnDfAgzTKU = false;}
      if(BPutNDpwJw == true){BPutNDpwJw = false;}
      if(INYzqzMLfC == true){INYzqzMLfC = false;}
      if(qoaTuENmtX == true){qoaTuENmtX = false;}
      if(ZrKNYyeijG == true){ZrKNYyeijG = false;}
      if(ROYWXxOEoB == true){ROYWXxOEoB = false;}
      if(UmnIdLnbzJ == true){UmnIdLnbzJ = false;}
      if(pDCkWPDLas == true){pDCkWPDLas = false;}
      if(UcbYjbQEuy == true){UcbYjbQEuy = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ASMBEQWJWX
{ 
  void xWWMqVoGrm()
  { 
      bool uhEDOgHMfO = false;
      bool UXEUHFRHpf = false;
      bool nBIRSrCWXn = false;
      bool cogYsRsPqU = false;
      bool PauZtstowZ = false;
      bool EFLIEsZlkW = false;
      bool ejnrlfpeao = false;
      bool MCQUQMGhWY = false;
      bool xNayiWcXUz = false;
      bool rGdTpelDnO = false;
      bool DONGMCoprq = false;
      bool mrOMZQJpIO = false;
      bool CSKaxGYwek = false;
      bool ZCsIUhATkg = false;
      bool hmecMAqQtY = false;
      bool VwuZLjElJG = false;
      bool crLHQjyGqq = false;
      bool QoEafPmgOx = false;
      bool OHCFPdHTxk = false;
      bool dNKeDpaXLb = false;
      string GqfiJXXjsF;
      string gWHfkuNmpT;
      string hRNRxueAwl;
      string jJynpAnhxK;
      string xHJgxWbzIw;
      string RhFMkodOkO;
      string rWmGVWXSok;
      string GfoeWaHfUa;
      string TfqAUBDkDB;
      string WZwhSZUCLZ;
      string yIJXolsapK;
      string gxsKtMfllT;
      string TlPmNgiHMw;
      string GETubKVTLh;
      string IcusUYflnF;
      string thQrFMsWJH;
      string CkOsDbdxhj;
      string pMaDLqqVRo;
      string AxTGCXnboP;
      string QyJDeWNCor;
      if(GqfiJXXjsF == yIJXolsapK){uhEDOgHMfO = true;}
      else if(yIJXolsapK == GqfiJXXjsF){DONGMCoprq = true;}
      if(gWHfkuNmpT == gxsKtMfllT){UXEUHFRHpf = true;}
      else if(gxsKtMfllT == gWHfkuNmpT){mrOMZQJpIO = true;}
      if(hRNRxueAwl == TlPmNgiHMw){nBIRSrCWXn = true;}
      else if(TlPmNgiHMw == hRNRxueAwl){CSKaxGYwek = true;}
      if(jJynpAnhxK == GETubKVTLh){cogYsRsPqU = true;}
      else if(GETubKVTLh == jJynpAnhxK){ZCsIUhATkg = true;}
      if(xHJgxWbzIw == IcusUYflnF){PauZtstowZ = true;}
      else if(IcusUYflnF == xHJgxWbzIw){hmecMAqQtY = true;}
      if(RhFMkodOkO == thQrFMsWJH){EFLIEsZlkW = true;}
      else if(thQrFMsWJH == RhFMkodOkO){VwuZLjElJG = true;}
      if(rWmGVWXSok == CkOsDbdxhj){ejnrlfpeao = true;}
      else if(CkOsDbdxhj == rWmGVWXSok){crLHQjyGqq = true;}
      if(GfoeWaHfUa == pMaDLqqVRo){MCQUQMGhWY = true;}
      if(TfqAUBDkDB == AxTGCXnboP){xNayiWcXUz = true;}
      if(WZwhSZUCLZ == QyJDeWNCor){rGdTpelDnO = true;}
      while(pMaDLqqVRo == GfoeWaHfUa){QoEafPmgOx = true;}
      while(AxTGCXnboP == AxTGCXnboP){OHCFPdHTxk = true;}
      while(QyJDeWNCor == QyJDeWNCor){dNKeDpaXLb = true;}
      if(uhEDOgHMfO == true){uhEDOgHMfO = false;}
      if(UXEUHFRHpf == true){UXEUHFRHpf = false;}
      if(nBIRSrCWXn == true){nBIRSrCWXn = false;}
      if(cogYsRsPqU == true){cogYsRsPqU = false;}
      if(PauZtstowZ == true){PauZtstowZ = false;}
      if(EFLIEsZlkW == true){EFLIEsZlkW = false;}
      if(ejnrlfpeao == true){ejnrlfpeao = false;}
      if(MCQUQMGhWY == true){MCQUQMGhWY = false;}
      if(xNayiWcXUz == true){xNayiWcXUz = false;}
      if(rGdTpelDnO == true){rGdTpelDnO = false;}
      if(DONGMCoprq == true){DONGMCoprq = false;}
      if(mrOMZQJpIO == true){mrOMZQJpIO = false;}
      if(CSKaxGYwek == true){CSKaxGYwek = false;}
      if(ZCsIUhATkg == true){ZCsIUhATkg = false;}
      if(hmecMAqQtY == true){hmecMAqQtY = false;}
      if(VwuZLjElJG == true){VwuZLjElJG = false;}
      if(crLHQjyGqq == true){crLHQjyGqq = false;}
      if(QoEafPmgOx == true){QoEafPmgOx = false;}
      if(OHCFPdHTxk == true){OHCFPdHTxk = false;}
      if(dNKeDpaXLb == true){dNKeDpaXLb = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class PPBQOEBZZI
{ 
  void nKqCFjKSPS()
  { 
      bool BkFPELrIHE = false;
      bool hIUsjnWSut = false;
      bool TfEqBPtfnQ = false;
      bool LDucqGnZOC = false;
      bool aWrKFzBOAH = false;
      bool ERaHlUfNOn = false;
      bool RMTqreMqCd = false;
      bool zwDrSwntlC = false;
      bool OeegMscnqW = false;
      bool ZZLakODBRa = false;
      bool lJMWHHOioo = false;
      bool glbLYmgldz = false;
      bool CYAfDZkcHD = false;
      bool slbKQFRIYF = false;
      bool dtzBonxzBy = false;
      bool hQXSYNIgSh = false;
      bool VlakutJRuT = false;
      bool UYNXUIYypP = false;
      bool blaIVcOJkC = false;
      bool wkoqjrSNGt = false;
      string qdFcNnPNGG;
      string LPXkWorKnF;
      string TqNnrLOKwz;
      string HZrHZfFyyb;
      string QENfFigADJ;
      string HpHaEAPRkB;
      string ZERGykWPLp;
      string jsMLnPdhiI;
      string eNHmrRRRDZ;
      string PmNwCLmGgT;
      string MRoMYNculo;
      string VhibWLxdOg;
      string SELlawfPJc;
      string bXBOwNYtNd;
      string nWeODhElZp;
      string OIrAAHFClb;
      string hNlJMygTfJ;
      string lCQjrwZxhr;
      string rtpSOSeAOn;
      string HVIkuakVBF;
      if(qdFcNnPNGG == MRoMYNculo){BkFPELrIHE = true;}
      else if(MRoMYNculo == qdFcNnPNGG){lJMWHHOioo = true;}
      if(LPXkWorKnF == VhibWLxdOg){hIUsjnWSut = true;}
      else if(VhibWLxdOg == LPXkWorKnF){glbLYmgldz = true;}
      if(TqNnrLOKwz == SELlawfPJc){TfEqBPtfnQ = true;}
      else if(SELlawfPJc == TqNnrLOKwz){CYAfDZkcHD = true;}
      if(HZrHZfFyyb == bXBOwNYtNd){LDucqGnZOC = true;}
      else if(bXBOwNYtNd == HZrHZfFyyb){slbKQFRIYF = true;}
      if(QENfFigADJ == nWeODhElZp){aWrKFzBOAH = true;}
      else if(nWeODhElZp == QENfFigADJ){dtzBonxzBy = true;}
      if(HpHaEAPRkB == OIrAAHFClb){ERaHlUfNOn = true;}
      else if(OIrAAHFClb == HpHaEAPRkB){hQXSYNIgSh = true;}
      if(ZERGykWPLp == hNlJMygTfJ){RMTqreMqCd = true;}
      else if(hNlJMygTfJ == ZERGykWPLp){VlakutJRuT = true;}
      if(jsMLnPdhiI == lCQjrwZxhr){zwDrSwntlC = true;}
      if(eNHmrRRRDZ == rtpSOSeAOn){OeegMscnqW = true;}
      if(PmNwCLmGgT == HVIkuakVBF){ZZLakODBRa = true;}
      while(lCQjrwZxhr == jsMLnPdhiI){UYNXUIYypP = true;}
      while(rtpSOSeAOn == rtpSOSeAOn){blaIVcOJkC = true;}
      while(HVIkuakVBF == HVIkuakVBF){wkoqjrSNGt = true;}
      if(BkFPELrIHE == true){BkFPELrIHE = false;}
      if(hIUsjnWSut == true){hIUsjnWSut = false;}
      if(TfEqBPtfnQ == true){TfEqBPtfnQ = false;}
      if(LDucqGnZOC == true){LDucqGnZOC = false;}
      if(aWrKFzBOAH == true){aWrKFzBOAH = false;}
      if(ERaHlUfNOn == true){ERaHlUfNOn = false;}
      if(RMTqreMqCd == true){RMTqreMqCd = false;}
      if(zwDrSwntlC == true){zwDrSwntlC = false;}
      if(OeegMscnqW == true){OeegMscnqW = false;}
      if(ZZLakODBRa == true){ZZLakODBRa = false;}
      if(lJMWHHOioo == true){lJMWHHOioo = false;}
      if(glbLYmgldz == true){glbLYmgldz = false;}
      if(CYAfDZkcHD == true){CYAfDZkcHD = false;}
      if(slbKQFRIYF == true){slbKQFRIYF = false;}
      if(dtzBonxzBy == true){dtzBonxzBy = false;}
      if(hQXSYNIgSh == true){hQXSYNIgSh = false;}
      if(VlakutJRuT == true){VlakutJRuT = false;}
      if(UYNXUIYypP == true){UYNXUIYypP = false;}
      if(blaIVcOJkC == true){blaIVcOJkC = false;}
      if(wkoqjrSNGt == true){wkoqjrSNGt = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NVIIKNKFTK
{ 
  void FbLfDdubTM()
  { 
      bool TzFARrUxIZ = false;
      bool eoMmRIVVsm = false;
      bool hpznuWfTJx = false;
      bool hSCGBWeVwC = false;
      bool xxOJQQqGFq = false;
      bool DePlOczREt = false;
      bool MAnLqLlFlt = false;
      bool czZppkbpWd = false;
      bool fYORsDDPrX = false;
      bool XNQBpwIDSF = false;
      bool dlTKNDFYwc = false;
      bool BJtaOmlVRn = false;
      bool HVMOWOhXAs = false;
      bool bYWRzsFDEp = false;
      bool xeTPVNTplC = false;
      bool KmkdxIYEny = false;
      bool VgYDBclbaU = false;
      bool hhRapcCxsI = false;
      bool uqlxinmsTq = false;
      bool UAOFTQFPNs = false;
      string RApAznOYxT;
      string CgFTiXwnju;
      string RfNrhhhXOi;
      string xIKmojHtpr;
      string lYWqWBnWKJ;
      string VOhLallxSQ;
      string FpJOWtWRez;
      string PCxrJsIWVf;
      string sLNFMhZyLC;
      string xKbZluPITc;
      string dWrKsTQoCh;
      string cPblXtcDBn;
      string TXFbCFaXGN;
      string dACrCWSCLr;
      string RjwdYghoBr;
      string SLGeIGQPIn;
      string srinEcNijB;
      string aZqIWiabDU;
      string WkWNuRtWiu;
      string FDtXzYBIYw;
      if(RApAznOYxT == dWrKsTQoCh){TzFARrUxIZ = true;}
      else if(dWrKsTQoCh == RApAznOYxT){dlTKNDFYwc = true;}
      if(CgFTiXwnju == cPblXtcDBn){eoMmRIVVsm = true;}
      else if(cPblXtcDBn == CgFTiXwnju){BJtaOmlVRn = true;}
      if(RfNrhhhXOi == TXFbCFaXGN){hpznuWfTJx = true;}
      else if(TXFbCFaXGN == RfNrhhhXOi){HVMOWOhXAs = true;}
      if(xIKmojHtpr == dACrCWSCLr){hSCGBWeVwC = true;}
      else if(dACrCWSCLr == xIKmojHtpr){bYWRzsFDEp = true;}
      if(lYWqWBnWKJ == RjwdYghoBr){xxOJQQqGFq = true;}
      else if(RjwdYghoBr == lYWqWBnWKJ){xeTPVNTplC = true;}
      if(VOhLallxSQ == SLGeIGQPIn){DePlOczREt = true;}
      else if(SLGeIGQPIn == VOhLallxSQ){KmkdxIYEny = true;}
      if(FpJOWtWRez == srinEcNijB){MAnLqLlFlt = true;}
      else if(srinEcNijB == FpJOWtWRez){VgYDBclbaU = true;}
      if(PCxrJsIWVf == aZqIWiabDU){czZppkbpWd = true;}
      if(sLNFMhZyLC == WkWNuRtWiu){fYORsDDPrX = true;}
      if(xKbZluPITc == FDtXzYBIYw){XNQBpwIDSF = true;}
      while(aZqIWiabDU == PCxrJsIWVf){hhRapcCxsI = true;}
      while(WkWNuRtWiu == WkWNuRtWiu){uqlxinmsTq = true;}
      while(FDtXzYBIYw == FDtXzYBIYw){UAOFTQFPNs = true;}
      if(TzFARrUxIZ == true){TzFARrUxIZ = false;}
      if(eoMmRIVVsm == true){eoMmRIVVsm = false;}
      if(hpznuWfTJx == true){hpznuWfTJx = false;}
      if(hSCGBWeVwC == true){hSCGBWeVwC = false;}
      if(xxOJQQqGFq == true){xxOJQQqGFq = false;}
      if(DePlOczREt == true){DePlOczREt = false;}
      if(MAnLqLlFlt == true){MAnLqLlFlt = false;}
      if(czZppkbpWd == true){czZppkbpWd = false;}
      if(fYORsDDPrX == true){fYORsDDPrX = false;}
      if(XNQBpwIDSF == true){XNQBpwIDSF = false;}
      if(dlTKNDFYwc == true){dlTKNDFYwc = false;}
      if(BJtaOmlVRn == true){BJtaOmlVRn = false;}
      if(HVMOWOhXAs == true){HVMOWOhXAs = false;}
      if(bYWRzsFDEp == true){bYWRzsFDEp = false;}
      if(xeTPVNTplC == true){xeTPVNTplC = false;}
      if(KmkdxIYEny == true){KmkdxIYEny = false;}
      if(VgYDBclbaU == true){VgYDBclbaU = false;}
      if(hhRapcCxsI == true){hhRapcCxsI = false;}
      if(uqlxinmsTq == true){uqlxinmsTq = false;}
      if(UAOFTQFPNs == true){UAOFTQFPNs = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KBWYWHWNFB
{ 
  void TnKPOXVDNX()
  { 
      bool YFNicGgVYh = false;
      bool PTaluneqij = false;
      bool XlJVucZXtx = false;
      bool IzsGHCTAEA = false;
      bool SeOzrMcDKD = false;
      bool hTGcXmwqnz = false;
      bool gIaaZQaUdr = false;
      bool TdlhQrWKdh = false;
      bool AVNqPnsPiU = false;
      bool GcLETJEnYa = false;
      bool lIZRiEgXMe = false;
      bool QlNGzLuiLk = false;
      bool WKkJlqShOG = false;
      bool nikCuqwuHK = false;
      bool HUDDDUOFEI = false;
      bool OUbnEbWPfr = false;
      bool kzJFqgALLH = false;
      bool OAbAcBHhFQ = false;
      bool cNcGrTGwxT = false;
      bool UfOPYILxMW = false;
      string ukuPbwUIaC;
      string kUsXZBRwnz;
      string MtpmkKAnCN;
      string fiwwCtbLJV;
      string aEYtDyaTfi;
      string CuitDBuHdR;
      string ufGNWBXwVf;
      string ZGlSYIUuim;
      string jfISKjzCsA;
      string zThrZtxLWC;
      string ZdyzJuIQVu;
      string gzXpgPXmCm;
      string BVDUgFYtrd;
      string KJYgMGhIJO;
      string bifxSzwqKK;
      string WKIjAEZcSD;
      string WOYqjXZpda;
      string gPdmKSoGCg;
      string IhigHAAngH;
      string fEoFrNKoNu;
      if(ukuPbwUIaC == ZdyzJuIQVu){YFNicGgVYh = true;}
      else if(ZdyzJuIQVu == ukuPbwUIaC){lIZRiEgXMe = true;}
      if(kUsXZBRwnz == gzXpgPXmCm){PTaluneqij = true;}
      else if(gzXpgPXmCm == kUsXZBRwnz){QlNGzLuiLk = true;}
      if(MtpmkKAnCN == BVDUgFYtrd){XlJVucZXtx = true;}
      else if(BVDUgFYtrd == MtpmkKAnCN){WKkJlqShOG = true;}
      if(fiwwCtbLJV == KJYgMGhIJO){IzsGHCTAEA = true;}
      else if(KJYgMGhIJO == fiwwCtbLJV){nikCuqwuHK = true;}
      if(aEYtDyaTfi == bifxSzwqKK){SeOzrMcDKD = true;}
      else if(bifxSzwqKK == aEYtDyaTfi){HUDDDUOFEI = true;}
      if(CuitDBuHdR == WKIjAEZcSD){hTGcXmwqnz = true;}
      else if(WKIjAEZcSD == CuitDBuHdR){OUbnEbWPfr = true;}
      if(ufGNWBXwVf == WOYqjXZpda){gIaaZQaUdr = true;}
      else if(WOYqjXZpda == ufGNWBXwVf){kzJFqgALLH = true;}
      if(ZGlSYIUuim == gPdmKSoGCg){TdlhQrWKdh = true;}
      if(jfISKjzCsA == IhigHAAngH){AVNqPnsPiU = true;}
      if(zThrZtxLWC == fEoFrNKoNu){GcLETJEnYa = true;}
      while(gPdmKSoGCg == ZGlSYIUuim){OAbAcBHhFQ = true;}
      while(IhigHAAngH == IhigHAAngH){cNcGrTGwxT = true;}
      while(fEoFrNKoNu == fEoFrNKoNu){UfOPYILxMW = true;}
      if(YFNicGgVYh == true){YFNicGgVYh = false;}
      if(PTaluneqij == true){PTaluneqij = false;}
      if(XlJVucZXtx == true){XlJVucZXtx = false;}
      if(IzsGHCTAEA == true){IzsGHCTAEA = false;}
      if(SeOzrMcDKD == true){SeOzrMcDKD = false;}
      if(hTGcXmwqnz == true){hTGcXmwqnz = false;}
      if(gIaaZQaUdr == true){gIaaZQaUdr = false;}
      if(TdlhQrWKdh == true){TdlhQrWKdh = false;}
      if(AVNqPnsPiU == true){AVNqPnsPiU = false;}
      if(GcLETJEnYa == true){GcLETJEnYa = false;}
      if(lIZRiEgXMe == true){lIZRiEgXMe = false;}
      if(QlNGzLuiLk == true){QlNGzLuiLk = false;}
      if(WKkJlqShOG == true){WKkJlqShOG = false;}
      if(nikCuqwuHK == true){nikCuqwuHK = false;}
      if(HUDDDUOFEI == true){HUDDDUOFEI = false;}
      if(OUbnEbWPfr == true){OUbnEbWPfr = false;}
      if(kzJFqgALLH == true){kzJFqgALLH = false;}
      if(OAbAcBHhFQ == true){OAbAcBHhFQ = false;}
      if(cNcGrTGwxT == true){cNcGrTGwxT = false;}
      if(UfOPYILxMW == true){UfOPYILxMW = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class IHQOCAGHOO
{ 
  void ricFrdPMAz()
  { 
      bool gWCXurQyfJ = false;
      bool KNxDydsefd = false;
      bool fbuXPtRkBF = false;
      bool iDnZWJhOwl = false;
      bool fKhtCgrjwa = false;
      bool VeBhwFadIB = false;
      bool ZfxaaHYWfn = false;
      bool udisxVsBVh = false;
      bool BhoZiDBGcQ = false;
      bool iEGuLANGSt = false;
      bool HUDHerwIja = false;
      bool MZIZzrpWqX = false;
      bool FStPFqXngx = false;
      bool IlhbZgegtq = false;
      bool GsVCEIwguT = false;
      bool JpPtnoanCn = false;
      bool UIamRZrgzR = false;
      bool NJcAQLwqKx = false;
      bool uprNmSWmmk = false;
      bool GqeGWySije = false;
      string LBOtyPBnnj;
      string rAYMXJSKQE;
      string LMMSQPcVZD;
      string WqrZTRaLkq;
      string dPRefhCPpR;
      string tZRMQynudh;
      string mfNkMBIyrG;
      string iGYYZuuhmi;
      string NNeIIwZeJV;
      string dehDDxBgRe;
      string YDWjinBrmU;
      string DGItUZAFbP;
      string acoVbDYVVb;
      string sNPIJJpuZU;
      string ugrnSXWmid;
      string xwFBDwOAHe;
      string PfWKbopAkK;
      string iFfGUskBDE;
      string gkmqexIOEN;
      string uomtFPKgMc;
      if(LBOtyPBnnj == YDWjinBrmU){gWCXurQyfJ = true;}
      else if(YDWjinBrmU == LBOtyPBnnj){HUDHerwIja = true;}
      if(rAYMXJSKQE == DGItUZAFbP){KNxDydsefd = true;}
      else if(DGItUZAFbP == rAYMXJSKQE){MZIZzrpWqX = true;}
      if(LMMSQPcVZD == acoVbDYVVb){fbuXPtRkBF = true;}
      else if(acoVbDYVVb == LMMSQPcVZD){FStPFqXngx = true;}
      if(WqrZTRaLkq == sNPIJJpuZU){iDnZWJhOwl = true;}
      else if(sNPIJJpuZU == WqrZTRaLkq){IlhbZgegtq = true;}
      if(dPRefhCPpR == ugrnSXWmid){fKhtCgrjwa = true;}
      else if(ugrnSXWmid == dPRefhCPpR){GsVCEIwguT = true;}
      if(tZRMQynudh == xwFBDwOAHe){VeBhwFadIB = true;}
      else if(xwFBDwOAHe == tZRMQynudh){JpPtnoanCn = true;}
      if(mfNkMBIyrG == PfWKbopAkK){ZfxaaHYWfn = true;}
      else if(PfWKbopAkK == mfNkMBIyrG){UIamRZrgzR = true;}
      if(iGYYZuuhmi == iFfGUskBDE){udisxVsBVh = true;}
      if(NNeIIwZeJV == gkmqexIOEN){BhoZiDBGcQ = true;}
      if(dehDDxBgRe == uomtFPKgMc){iEGuLANGSt = true;}
      while(iFfGUskBDE == iGYYZuuhmi){NJcAQLwqKx = true;}
      while(gkmqexIOEN == gkmqexIOEN){uprNmSWmmk = true;}
      while(uomtFPKgMc == uomtFPKgMc){GqeGWySije = true;}
      if(gWCXurQyfJ == true){gWCXurQyfJ = false;}
      if(KNxDydsefd == true){KNxDydsefd = false;}
      if(fbuXPtRkBF == true){fbuXPtRkBF = false;}
      if(iDnZWJhOwl == true){iDnZWJhOwl = false;}
      if(fKhtCgrjwa == true){fKhtCgrjwa = false;}
      if(VeBhwFadIB == true){VeBhwFadIB = false;}
      if(ZfxaaHYWfn == true){ZfxaaHYWfn = false;}
      if(udisxVsBVh == true){udisxVsBVh = false;}
      if(BhoZiDBGcQ == true){BhoZiDBGcQ = false;}
      if(iEGuLANGSt == true){iEGuLANGSt = false;}
      if(HUDHerwIja == true){HUDHerwIja = false;}
      if(MZIZzrpWqX == true){MZIZzrpWqX = false;}
      if(FStPFqXngx == true){FStPFqXngx = false;}
      if(IlhbZgegtq == true){IlhbZgegtq = false;}
      if(GsVCEIwguT == true){GsVCEIwguT = false;}
      if(JpPtnoanCn == true){JpPtnoanCn = false;}
      if(UIamRZrgzR == true){UIamRZrgzR = false;}
      if(NJcAQLwqKx == true){NJcAQLwqKx = false;}
      if(uprNmSWmmk == true){uprNmSWmmk = false;}
      if(GqeGWySije == true){GqeGWySije = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class YDKWBKNBNP
{ 
  void DmrKNejRJE()
  { 
      bool lUpzZaCHZU = false;
      bool jALhVDmbFn = false;
      bool EkOOYkbWli = false;
      bool KzMQWGCxfC = false;
      bool abWKkpGyLl = false;
      bool WIneoAGbZn = false;
      bool zfVpJjntLP = false;
      bool AowDMbNDci = false;
      bool edKzMBLgaW = false;
      bool zaXqDuGgAA = false;
      bool BLlukhLkRM = false;
      bool lDyqJVUWdH = false;
      bool YGSoQZRFAQ = false;
      bool VgCcgTqPNd = false;
      bool paPOjwcfhh = false;
      bool ZjDrburxeL = false;
      bool QGjAqFAVKp = false;
      bool dFIzRxqEET = false;
      bool xkWSqlcJAl = false;
      bool asjCNipOXd = false;
      string AohfUMhNIs;
      string NpaDmZYaeP;
      string baQMuPLJIt;
      string KLuWQqbuIX;
      string nyLqiuyEQm;
      string sQZWhYXUjF;
      string AYFOiCtDmw;
      string xdRHbdPYdA;
      string RHrUBtlXnr;
      string SGKDWPrwIg;
      string AtRUoPySuD;
      string IyqjmsfjNo;
      string PWOtcqSNjM;
      string lTQwzWoXfl;
      string uTcjiTPaQd;
      string iHiTQdeWqy;
      string wkhiRohtjQ;
      string DIEbtwseLI;
      string brxFcdjxfV;
      string qRfhoKksSt;
      if(AohfUMhNIs == AtRUoPySuD){lUpzZaCHZU = true;}
      else if(AtRUoPySuD == AohfUMhNIs){BLlukhLkRM = true;}
      if(NpaDmZYaeP == IyqjmsfjNo){jALhVDmbFn = true;}
      else if(IyqjmsfjNo == NpaDmZYaeP){lDyqJVUWdH = true;}
      if(baQMuPLJIt == PWOtcqSNjM){EkOOYkbWli = true;}
      else if(PWOtcqSNjM == baQMuPLJIt){YGSoQZRFAQ = true;}
      if(KLuWQqbuIX == lTQwzWoXfl){KzMQWGCxfC = true;}
      else if(lTQwzWoXfl == KLuWQqbuIX){VgCcgTqPNd = true;}
      if(nyLqiuyEQm == uTcjiTPaQd){abWKkpGyLl = true;}
      else if(uTcjiTPaQd == nyLqiuyEQm){paPOjwcfhh = true;}
      if(sQZWhYXUjF == iHiTQdeWqy){WIneoAGbZn = true;}
      else if(iHiTQdeWqy == sQZWhYXUjF){ZjDrburxeL = true;}
      if(AYFOiCtDmw == wkhiRohtjQ){zfVpJjntLP = true;}
      else if(wkhiRohtjQ == AYFOiCtDmw){QGjAqFAVKp = true;}
      if(xdRHbdPYdA == DIEbtwseLI){AowDMbNDci = true;}
      if(RHrUBtlXnr == brxFcdjxfV){edKzMBLgaW = true;}
      if(SGKDWPrwIg == qRfhoKksSt){zaXqDuGgAA = true;}
      while(DIEbtwseLI == xdRHbdPYdA){dFIzRxqEET = true;}
      while(brxFcdjxfV == brxFcdjxfV){xkWSqlcJAl = true;}
      while(qRfhoKksSt == qRfhoKksSt){asjCNipOXd = true;}
      if(lUpzZaCHZU == true){lUpzZaCHZU = false;}
      if(jALhVDmbFn == true){jALhVDmbFn = false;}
      if(EkOOYkbWli == true){EkOOYkbWli = false;}
      if(KzMQWGCxfC == true){KzMQWGCxfC = false;}
      if(abWKkpGyLl == true){abWKkpGyLl = false;}
      if(WIneoAGbZn == true){WIneoAGbZn = false;}
      if(zfVpJjntLP == true){zfVpJjntLP = false;}
      if(AowDMbNDci == true){AowDMbNDci = false;}
      if(edKzMBLgaW == true){edKzMBLgaW = false;}
      if(zaXqDuGgAA == true){zaXqDuGgAA = false;}
      if(BLlukhLkRM == true){BLlukhLkRM = false;}
      if(lDyqJVUWdH == true){lDyqJVUWdH = false;}
      if(YGSoQZRFAQ == true){YGSoQZRFAQ = false;}
      if(VgCcgTqPNd == true){VgCcgTqPNd = false;}
      if(paPOjwcfhh == true){paPOjwcfhh = false;}
      if(ZjDrburxeL == true){ZjDrburxeL = false;}
      if(QGjAqFAVKp == true){QGjAqFAVKp = false;}
      if(dFIzRxqEET == true){dFIzRxqEET = false;}
      if(xkWSqlcJAl == true){xkWSqlcJAl = false;}
      if(asjCNipOXd == true){asjCNipOXd = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class SNLZLXEMGL
{ 
  void AKqdztecZI()
  { 
      bool rNYhjEGQxM = false;
      bool SeZbjAzYps = false;
      bool JcOWFxdXBG = false;
      bool WrAhpVnnax = false;
      bool HJKRkhMTMr = false;
      bool zLBifOzCbm = false;
      bool ZhOppZdrlO = false;
      bool LLyrlASwkX = false;
      bool jKLIHKpRCa = false;
      bool VkLnXYtTbK = false;
      bool DTgxgmNJwe = false;
      bool uDRECOWGhe = false;
      bool jMpKsZAKTM = false;
      bool FyUZUQdNbU = false;
      bool wfUWfSTHkI = false;
      bool CnlwaqdVEG = false;
      bool mDojZwhrjk = false;
      bool kIeNhDqTea = false;
      bool GrYLJRMXyc = false;
      bool lHJVgsqkwi = false;
      string HxrmrUtsoO;
      string dGgDzfnnKO;
      string hqKDcKTkdF;
      string XRckLMnrYp;
      string UctUHzRmIC;
      string WJiTDQlnKh;
      string BJOipLdurl;
      string quOTsEwpwc;
      string rfcbhOiJht;
      string ZAhqoLOLdw;
      string VVuVKbtOZy;
      string UwrgoUglAa;
      string bTNxLqNXeg;
      string pjLzGVkGQG;
      string UUjjZcHABT;
      string ebKDUCIxKT;
      string NVirHjksaS;
      string jZVtFHjcXh;
      string iNVKCNZHeE;
      string Nmuxsykqbo;
      if(HxrmrUtsoO == VVuVKbtOZy){rNYhjEGQxM = true;}
      else if(VVuVKbtOZy == HxrmrUtsoO){DTgxgmNJwe = true;}
      if(dGgDzfnnKO == UwrgoUglAa){SeZbjAzYps = true;}
      else if(UwrgoUglAa == dGgDzfnnKO){uDRECOWGhe = true;}
      if(hqKDcKTkdF == bTNxLqNXeg){JcOWFxdXBG = true;}
      else if(bTNxLqNXeg == hqKDcKTkdF){jMpKsZAKTM = true;}
      if(XRckLMnrYp == pjLzGVkGQG){WrAhpVnnax = true;}
      else if(pjLzGVkGQG == XRckLMnrYp){FyUZUQdNbU = true;}
      if(UctUHzRmIC == UUjjZcHABT){HJKRkhMTMr = true;}
      else if(UUjjZcHABT == UctUHzRmIC){wfUWfSTHkI = true;}
      if(WJiTDQlnKh == ebKDUCIxKT){zLBifOzCbm = true;}
      else if(ebKDUCIxKT == WJiTDQlnKh){CnlwaqdVEG = true;}
      if(BJOipLdurl == NVirHjksaS){ZhOppZdrlO = true;}
      else if(NVirHjksaS == BJOipLdurl){mDojZwhrjk = true;}
      if(quOTsEwpwc == jZVtFHjcXh){LLyrlASwkX = true;}
      if(rfcbhOiJht == iNVKCNZHeE){jKLIHKpRCa = true;}
      if(ZAhqoLOLdw == Nmuxsykqbo){VkLnXYtTbK = true;}
      while(jZVtFHjcXh == quOTsEwpwc){kIeNhDqTea = true;}
      while(iNVKCNZHeE == iNVKCNZHeE){GrYLJRMXyc = true;}
      while(Nmuxsykqbo == Nmuxsykqbo){lHJVgsqkwi = true;}
      if(rNYhjEGQxM == true){rNYhjEGQxM = false;}
      if(SeZbjAzYps == true){SeZbjAzYps = false;}
      if(JcOWFxdXBG == true){JcOWFxdXBG = false;}
      if(WrAhpVnnax == true){WrAhpVnnax = false;}
      if(HJKRkhMTMr == true){HJKRkhMTMr = false;}
      if(zLBifOzCbm == true){zLBifOzCbm = false;}
      if(ZhOppZdrlO == true){ZhOppZdrlO = false;}
      if(LLyrlASwkX == true){LLyrlASwkX = false;}
      if(jKLIHKpRCa == true){jKLIHKpRCa = false;}
      if(VkLnXYtTbK == true){VkLnXYtTbK = false;}
      if(DTgxgmNJwe == true){DTgxgmNJwe = false;}
      if(uDRECOWGhe == true){uDRECOWGhe = false;}
      if(jMpKsZAKTM == true){jMpKsZAKTM = false;}
      if(FyUZUQdNbU == true){FyUZUQdNbU = false;}
      if(wfUWfSTHkI == true){wfUWfSTHkI = false;}
      if(CnlwaqdVEG == true){CnlwaqdVEG = false;}
      if(mDojZwhrjk == true){mDojZwhrjk = false;}
      if(kIeNhDqTea == true){kIeNhDqTea = false;}
      if(GrYLJRMXyc == true){GrYLJRMXyc = false;}
      if(lHJVgsqkwi == true){lHJVgsqkwi = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZCBSYUKNKB
{ 
  void ruOSiHrNOV()
  { 
      bool tRpZEcCBBH = false;
      bool YsjCIUEbsU = false;
      bool oCxzFXjLPw = false;
      bool TyCOLlyPmK = false;
      bool kmYoXPYsts = false;
      bool PZSKoqUqTg = false;
      bool spkEnapVqW = false;
      bool WnohjCdHao = false;
      bool ypQVfseqYT = false;
      bool RHuGQtBTBx = false;
      bool YDCXWakjam = false;
      bool NMQdhDZPYt = false;
      bool uywWEOrhRp = false;
      bool BJGcPtmszF = false;
      bool IuQlFEqRXM = false;
      bool UkuGCZGfSH = false;
      bool hCSjlDGmNm = false;
      bool PfPSlAhKQz = false;
      bool XaKEUPOzao = false;
      bool nHZqWFbfzC = false;
      string EKwjjzQIJr;
      string PRjNSHKlmF;
      string YDCbjCIrfs;
      string bFpMGzEQUz;
      string PYANfOUnDi;
      string gokTMAITzg;
      string pXbPQKaWHT;
      string DsRmoADnwU;
      string hUnwMlTWAF;
      string qjRmIrwjwU;
      string nxfjfmXydM;
      string xTWdzVieDe;
      string qDVCbSbddx;
      string DTqbgjnDQY;
      string OVsnIWNmnl;
      string IQfRdjadHN;
      string KtTgIccuwO;
      string heOdQVKhsF;
      string EooqnmnZxx;
      string IJuDOTBLfe;
      if(EKwjjzQIJr == nxfjfmXydM){tRpZEcCBBH = true;}
      else if(nxfjfmXydM == EKwjjzQIJr){YDCXWakjam = true;}
      if(PRjNSHKlmF == xTWdzVieDe){YsjCIUEbsU = true;}
      else if(xTWdzVieDe == PRjNSHKlmF){NMQdhDZPYt = true;}
      if(YDCbjCIrfs == qDVCbSbddx){oCxzFXjLPw = true;}
      else if(qDVCbSbddx == YDCbjCIrfs){uywWEOrhRp = true;}
      if(bFpMGzEQUz == DTqbgjnDQY){TyCOLlyPmK = true;}
      else if(DTqbgjnDQY == bFpMGzEQUz){BJGcPtmszF = true;}
      if(PYANfOUnDi == OVsnIWNmnl){kmYoXPYsts = true;}
      else if(OVsnIWNmnl == PYANfOUnDi){IuQlFEqRXM = true;}
      if(gokTMAITzg == IQfRdjadHN){PZSKoqUqTg = true;}
      else if(IQfRdjadHN == gokTMAITzg){UkuGCZGfSH = true;}
      if(pXbPQKaWHT == KtTgIccuwO){spkEnapVqW = true;}
      else if(KtTgIccuwO == pXbPQKaWHT){hCSjlDGmNm = true;}
      if(DsRmoADnwU == heOdQVKhsF){WnohjCdHao = true;}
      if(hUnwMlTWAF == EooqnmnZxx){ypQVfseqYT = true;}
      if(qjRmIrwjwU == IJuDOTBLfe){RHuGQtBTBx = true;}
      while(heOdQVKhsF == DsRmoADnwU){PfPSlAhKQz = true;}
      while(EooqnmnZxx == EooqnmnZxx){XaKEUPOzao = true;}
      while(IJuDOTBLfe == IJuDOTBLfe){nHZqWFbfzC = true;}
      if(tRpZEcCBBH == true){tRpZEcCBBH = false;}
      if(YsjCIUEbsU == true){YsjCIUEbsU = false;}
      if(oCxzFXjLPw == true){oCxzFXjLPw = false;}
      if(TyCOLlyPmK == true){TyCOLlyPmK = false;}
      if(kmYoXPYsts == true){kmYoXPYsts = false;}
      if(PZSKoqUqTg == true){PZSKoqUqTg = false;}
      if(spkEnapVqW == true){spkEnapVqW = false;}
      if(WnohjCdHao == true){WnohjCdHao = false;}
      if(ypQVfseqYT == true){ypQVfseqYT = false;}
      if(RHuGQtBTBx == true){RHuGQtBTBx = false;}
      if(YDCXWakjam == true){YDCXWakjam = false;}
      if(NMQdhDZPYt == true){NMQdhDZPYt = false;}
      if(uywWEOrhRp == true){uywWEOrhRp = false;}
      if(BJGcPtmszF == true){BJGcPtmszF = false;}
      if(IuQlFEqRXM == true){IuQlFEqRXM = false;}
      if(UkuGCZGfSH == true){UkuGCZGfSH = false;}
      if(hCSjlDGmNm == true){hCSjlDGmNm = false;}
      if(PfPSlAhKQz == true){PfPSlAhKQz = false;}
      if(XaKEUPOzao == true){XaKEUPOzao = false;}
      if(nHZqWFbfzC == true){nHZqWFbfzC = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VKEPBBTRLJ
{ 
  void EBiHkkydyF()
  { 
      bool sphDkOqDxV = false;
      bool GzlkmpsOSU = false;
      bool zrDpaePUld = false;
      bool EJdjdbLXcy = false;
      bool GBwAJhGZBa = false;
      bool pTyqzAlqbF = false;
      bool zWQTQxUCHK = false;
      bool eUkTOywDgr = false;
      bool YoSKnjKljc = false;
      bool pTwXzijbiL = false;
      bool MDSpwVidcB = false;
      bool EiKigJDFeg = false;
      bool liRdUKYNUe = false;
      bool NTTUeUWbOR = false;
      bool BgJhYGJMpo = false;
      bool eexCYdknVl = false;
      bool xUIynjPoDL = false;
      bool mbqNfFYjEN = false;
      bool AMWezhWQwB = false;
      bool jhGbUfXKtn = false;
      string ESEeWGqMlk;
      string xOaWYTgrcy;
      string sMrLUGtQZN;
      string FiGMNyPEJI;
      string KqmIWOYUkr;
      string trVXSqasKP;
      string MZlYeNHOJZ;
      string EbiTnCZinF;
      string hZehSNNitV;
      string XVttwjENcI;
      string GKKLnRJBjP;
      string RrFFgmtDFR;
      string AYUnChhnXD;
      string yMFeyXOTtB;
      string EMydEePnMJ;
      string oAHLIhpVIF;
      string jUcgRElZGJ;
      string XaJENmapjW;
      string RsbONxEtVQ;
      string hrLoRiiRLz;
      if(ESEeWGqMlk == GKKLnRJBjP){sphDkOqDxV = true;}
      else if(GKKLnRJBjP == ESEeWGqMlk){MDSpwVidcB = true;}
      if(xOaWYTgrcy == RrFFgmtDFR){GzlkmpsOSU = true;}
      else if(RrFFgmtDFR == xOaWYTgrcy){EiKigJDFeg = true;}
      if(sMrLUGtQZN == AYUnChhnXD){zrDpaePUld = true;}
      else if(AYUnChhnXD == sMrLUGtQZN){liRdUKYNUe = true;}
      if(FiGMNyPEJI == yMFeyXOTtB){EJdjdbLXcy = true;}
      else if(yMFeyXOTtB == FiGMNyPEJI){NTTUeUWbOR = true;}
      if(KqmIWOYUkr == EMydEePnMJ){GBwAJhGZBa = true;}
      else if(EMydEePnMJ == KqmIWOYUkr){BgJhYGJMpo = true;}
      if(trVXSqasKP == oAHLIhpVIF){pTyqzAlqbF = true;}
      else if(oAHLIhpVIF == trVXSqasKP){eexCYdknVl = true;}
      if(MZlYeNHOJZ == jUcgRElZGJ){zWQTQxUCHK = true;}
      else if(jUcgRElZGJ == MZlYeNHOJZ){xUIynjPoDL = true;}
      if(EbiTnCZinF == XaJENmapjW){eUkTOywDgr = true;}
      if(hZehSNNitV == RsbONxEtVQ){YoSKnjKljc = true;}
      if(XVttwjENcI == hrLoRiiRLz){pTwXzijbiL = true;}
      while(XaJENmapjW == EbiTnCZinF){mbqNfFYjEN = true;}
      while(RsbONxEtVQ == RsbONxEtVQ){AMWezhWQwB = true;}
      while(hrLoRiiRLz == hrLoRiiRLz){jhGbUfXKtn = true;}
      if(sphDkOqDxV == true){sphDkOqDxV = false;}
      if(GzlkmpsOSU == true){GzlkmpsOSU = false;}
      if(zrDpaePUld == true){zrDpaePUld = false;}
      if(EJdjdbLXcy == true){EJdjdbLXcy = false;}
      if(GBwAJhGZBa == true){GBwAJhGZBa = false;}
      if(pTyqzAlqbF == true){pTyqzAlqbF = false;}
      if(zWQTQxUCHK == true){zWQTQxUCHK = false;}
      if(eUkTOywDgr == true){eUkTOywDgr = false;}
      if(YoSKnjKljc == true){YoSKnjKljc = false;}
      if(pTwXzijbiL == true){pTwXzijbiL = false;}
      if(MDSpwVidcB == true){MDSpwVidcB = false;}
      if(EiKigJDFeg == true){EiKigJDFeg = false;}
      if(liRdUKYNUe == true){liRdUKYNUe = false;}
      if(NTTUeUWbOR == true){NTTUeUWbOR = false;}
      if(BgJhYGJMpo == true){BgJhYGJMpo = false;}
      if(eexCYdknVl == true){eexCYdknVl = false;}
      if(xUIynjPoDL == true){xUIynjPoDL = false;}
      if(mbqNfFYjEN == true){mbqNfFYjEN = false;}
      if(AMWezhWQwB == true){AMWezhWQwB = false;}
      if(jhGbUfXKtn == true){jhGbUfXKtn = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class XFCPIYBIQU
{ 
  void KxBHVyjcux()
  { 
      bool bVBwEJgcOU = false;
      bool fDEITYWrWM = false;
      bool FTiZIjaSMC = false;
      bool nWnThOebnw = false;
      bool IXDisYXuho = false;
      bool atSxOImanT = false;
      bool hNPLfIOkZE = false;
      bool tuDaDhPzfi = false;
      bool MekXatEqEZ = false;
      bool aDmUNuHqCj = false;
      bool JmVdtepqsg = false;
      bool rntEHDcqkl = false;
      bool DXfAdpHiMH = false;
      bool ZXwwZZJHdu = false;
      bool DMxXglIiMK = false;
      bool RPGQfkGUzD = false;
      bool XGekGTryeh = false;
      bool sVNoipaITq = false;
      bool hYWCoJRfPn = false;
      bool ToHZXwhLQM = false;
      string TmIxVHQZte;
      string MZpCobazjo;
      string eEgVnRqDts;
      string smCYAljbzT;
      string DJAsWwcKrp;
      string lAOKROGlJD;
      string QMeBkZWRBD;
      string rzAPlVDnyq;
      string ofLEGrYqHl;
      string fxSuDpdzHx;
      string YLJoymAyYu;
      string KSnROjBaur;
      string BfJPWtgFMu;
      string rEgASdWVFT;
      string rZSLaNJAgS;
      string oCDOwsBsRD;
      string FulcCGlmzj;
      string NeUaAaMxnJ;
      string jfXsGpAREZ;
      string bcEBNxmoJU;
      if(TmIxVHQZte == YLJoymAyYu){bVBwEJgcOU = true;}
      else if(YLJoymAyYu == TmIxVHQZte){JmVdtepqsg = true;}
      if(MZpCobazjo == KSnROjBaur){fDEITYWrWM = true;}
      else if(KSnROjBaur == MZpCobazjo){rntEHDcqkl = true;}
      if(eEgVnRqDts == BfJPWtgFMu){FTiZIjaSMC = true;}
      else if(BfJPWtgFMu == eEgVnRqDts){DXfAdpHiMH = true;}
      if(smCYAljbzT == rEgASdWVFT){nWnThOebnw = true;}
      else if(rEgASdWVFT == smCYAljbzT){ZXwwZZJHdu = true;}
      if(DJAsWwcKrp == rZSLaNJAgS){IXDisYXuho = true;}
      else if(rZSLaNJAgS == DJAsWwcKrp){DMxXglIiMK = true;}
      if(lAOKROGlJD == oCDOwsBsRD){atSxOImanT = true;}
      else if(oCDOwsBsRD == lAOKROGlJD){RPGQfkGUzD = true;}
      if(QMeBkZWRBD == FulcCGlmzj){hNPLfIOkZE = true;}
      else if(FulcCGlmzj == QMeBkZWRBD){XGekGTryeh = true;}
      if(rzAPlVDnyq == NeUaAaMxnJ){tuDaDhPzfi = true;}
      if(ofLEGrYqHl == jfXsGpAREZ){MekXatEqEZ = true;}
      if(fxSuDpdzHx == bcEBNxmoJU){aDmUNuHqCj = true;}
      while(NeUaAaMxnJ == rzAPlVDnyq){sVNoipaITq = true;}
      while(jfXsGpAREZ == jfXsGpAREZ){hYWCoJRfPn = true;}
      while(bcEBNxmoJU == bcEBNxmoJU){ToHZXwhLQM = true;}
      if(bVBwEJgcOU == true){bVBwEJgcOU = false;}
      if(fDEITYWrWM == true){fDEITYWrWM = false;}
      if(FTiZIjaSMC == true){FTiZIjaSMC = false;}
      if(nWnThOebnw == true){nWnThOebnw = false;}
      if(IXDisYXuho == true){IXDisYXuho = false;}
      if(atSxOImanT == true){atSxOImanT = false;}
      if(hNPLfIOkZE == true){hNPLfIOkZE = false;}
      if(tuDaDhPzfi == true){tuDaDhPzfi = false;}
      if(MekXatEqEZ == true){MekXatEqEZ = false;}
      if(aDmUNuHqCj == true){aDmUNuHqCj = false;}
      if(JmVdtepqsg == true){JmVdtepqsg = false;}
      if(rntEHDcqkl == true){rntEHDcqkl = false;}
      if(DXfAdpHiMH == true){DXfAdpHiMH = false;}
      if(ZXwwZZJHdu == true){ZXwwZZJHdu = false;}
      if(DMxXglIiMK == true){DMxXglIiMK = false;}
      if(RPGQfkGUzD == true){RPGQfkGUzD = false;}
      if(XGekGTryeh == true){XGekGTryeh = false;}
      if(sVNoipaITq == true){sVNoipaITq = false;}
      if(hYWCoJRfPn == true){hYWCoJRfPn = false;}
      if(ToHZXwhLQM == true){ToHZXwhLQM = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class IFGTPKIWSQ
{ 
  void yBaefqpXQL()
  { 
      bool PYeycdVPqM = false;
      bool LfHoVuBXPf = false;
      bool oXAhwbLhTp = false;
      bool awCRoVQAGd = false;
      bool zBcfsZwgpV = false;
      bool atkUDrYxCA = false;
      bool SmuAlPdpzz = false;
      bool dARkfmtUnW = false;
      bool edryOfKhOx = false;
      bool xSyFLMGcsd = false;
      bool wGhpqacznQ = false;
      bool yDIfUnuxkF = false;
      bool EubCCKBLZT = false;
      bool gQaJZXwBGm = false;
      bool DLsxysKSjX = false;
      bool QIqnZAeenf = false;
      bool ZQZUjgbyFx = false;
      bool qtqxCrCdDI = false;
      bool AhaCDXZmjZ = false;
      bool sQMZNlkYNQ = false;
      string zXxjkHArQE;
      string QwEpbyRJSW;
      string wNRieOGZLJ;
      string baAHJjYcPN;
      string mUBhMekIzD;
      string txDeQkGOEV;
      string BZfHbouwgq;
      string YntEibMjVA;
      string dyZHOeXVqX;
      string dKYsMNotFX;
      string jnaIeGCyuo;
      string YVrErDAXsw;
      string RYFINNPBrV;
      string mSdyQoWRcH;
      string cYaCyHSTBJ;
      string MQAJFKiRuQ;
      string qAGYBGqsnC;
      string ghtkSsyCKF;
      string uwziIVRTrF;
      string GEWMQbRBAr;
      if(zXxjkHArQE == jnaIeGCyuo){PYeycdVPqM = true;}
      else if(jnaIeGCyuo == zXxjkHArQE){wGhpqacznQ = true;}
      if(QwEpbyRJSW == YVrErDAXsw){LfHoVuBXPf = true;}
      else if(YVrErDAXsw == QwEpbyRJSW){yDIfUnuxkF = true;}
      if(wNRieOGZLJ == RYFINNPBrV){oXAhwbLhTp = true;}
      else if(RYFINNPBrV == wNRieOGZLJ){EubCCKBLZT = true;}
      if(baAHJjYcPN == mSdyQoWRcH){awCRoVQAGd = true;}
      else if(mSdyQoWRcH == baAHJjYcPN){gQaJZXwBGm = true;}
      if(mUBhMekIzD == cYaCyHSTBJ){zBcfsZwgpV = true;}
      else if(cYaCyHSTBJ == mUBhMekIzD){DLsxysKSjX = true;}
      if(txDeQkGOEV == MQAJFKiRuQ){atkUDrYxCA = true;}
      else if(MQAJFKiRuQ == txDeQkGOEV){QIqnZAeenf = true;}
      if(BZfHbouwgq == qAGYBGqsnC){SmuAlPdpzz = true;}
      else if(qAGYBGqsnC == BZfHbouwgq){ZQZUjgbyFx = true;}
      if(YntEibMjVA == ghtkSsyCKF){dARkfmtUnW = true;}
      if(dyZHOeXVqX == uwziIVRTrF){edryOfKhOx = true;}
      if(dKYsMNotFX == GEWMQbRBAr){xSyFLMGcsd = true;}
      while(ghtkSsyCKF == YntEibMjVA){qtqxCrCdDI = true;}
      while(uwziIVRTrF == uwziIVRTrF){AhaCDXZmjZ = true;}
      while(GEWMQbRBAr == GEWMQbRBAr){sQMZNlkYNQ = true;}
      if(PYeycdVPqM == true){PYeycdVPqM = false;}
      if(LfHoVuBXPf == true){LfHoVuBXPf = false;}
      if(oXAhwbLhTp == true){oXAhwbLhTp = false;}
      if(awCRoVQAGd == true){awCRoVQAGd = false;}
      if(zBcfsZwgpV == true){zBcfsZwgpV = false;}
      if(atkUDrYxCA == true){atkUDrYxCA = false;}
      if(SmuAlPdpzz == true){SmuAlPdpzz = false;}
      if(dARkfmtUnW == true){dARkfmtUnW = false;}
      if(edryOfKhOx == true){edryOfKhOx = false;}
      if(xSyFLMGcsd == true){xSyFLMGcsd = false;}
      if(wGhpqacznQ == true){wGhpqacznQ = false;}
      if(yDIfUnuxkF == true){yDIfUnuxkF = false;}
      if(EubCCKBLZT == true){EubCCKBLZT = false;}
      if(gQaJZXwBGm == true){gQaJZXwBGm = false;}
      if(DLsxysKSjX == true){DLsxysKSjX = false;}
      if(QIqnZAeenf == true){QIqnZAeenf = false;}
      if(ZQZUjgbyFx == true){ZQZUjgbyFx = false;}
      if(qtqxCrCdDI == true){qtqxCrCdDI = false;}
      if(AhaCDXZmjZ == true){AhaCDXZmjZ = false;}
      if(sQMZNlkYNQ == true){sQMZNlkYNQ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NBYNKGQRCA
{ 
  void DlYWtFHBEJ()
  { 
      bool GDIpGfiHsN = false;
      bool LcbhRZLxQG = false;
      bool zqsZJjFTCw = false;
      bool JKkTDMGnVq = false;
      bool QHHxhRLXrW = false;
      bool kiVIAOKhfV = false;
      bool ZPExEArHdE = false;
      bool TWaZpqjurq = false;
      bool YhafLaobTo = false;
      bool CXIAQBDKsg = false;
      bool ZEsnDOHwSI = false;
      bool eOpnxrnYlc = false;
      bool LPnoCuOaJn = false;
      bool nYCKRTomcw = false;
      bool TsIKJmhTCX = false;
      bool NdYPUJPEkH = false;
      bool AfleUGMeWc = false;
      bool DBwoJSGdeO = false;
      bool MtXkZKzlpH = false;
      bool EkBUumGdoD = false;
      string QElktKLpwy;
      string wEiqVTEVGf;
      string QNMAZrjzOr;
      string ONAREbPLZM;
      string NOyDjmSTxL;
      string YMaUXCrlKC;
      string ZLbEiiEQlt;
      string DrwtRBPJQm;
      string ZQBCSCmesC;
      string idAWmCmsTA;
      string iBgDIgPgDV;
      string liUXcIJYlf;
      string wYXjKZqZGP;
      string OWoPDXjytl;
      string mGaWPAIcte;
      string rNHmEJcwri;
      string opZnHPsETE;
      string XLezspxAUK;
      string QOebWAPBLt;
      string tmFKxwfmPA;
      if(QElktKLpwy == iBgDIgPgDV){GDIpGfiHsN = true;}
      else if(iBgDIgPgDV == QElktKLpwy){ZEsnDOHwSI = true;}
      if(wEiqVTEVGf == liUXcIJYlf){LcbhRZLxQG = true;}
      else if(liUXcIJYlf == wEiqVTEVGf){eOpnxrnYlc = true;}
      if(QNMAZrjzOr == wYXjKZqZGP){zqsZJjFTCw = true;}
      else if(wYXjKZqZGP == QNMAZrjzOr){LPnoCuOaJn = true;}
      if(ONAREbPLZM == OWoPDXjytl){JKkTDMGnVq = true;}
      else if(OWoPDXjytl == ONAREbPLZM){nYCKRTomcw = true;}
      if(NOyDjmSTxL == mGaWPAIcte){QHHxhRLXrW = true;}
      else if(mGaWPAIcte == NOyDjmSTxL){TsIKJmhTCX = true;}
      if(YMaUXCrlKC == rNHmEJcwri){kiVIAOKhfV = true;}
      else if(rNHmEJcwri == YMaUXCrlKC){NdYPUJPEkH = true;}
      if(ZLbEiiEQlt == opZnHPsETE){ZPExEArHdE = true;}
      else if(opZnHPsETE == ZLbEiiEQlt){AfleUGMeWc = true;}
      if(DrwtRBPJQm == XLezspxAUK){TWaZpqjurq = true;}
      if(ZQBCSCmesC == QOebWAPBLt){YhafLaobTo = true;}
      if(idAWmCmsTA == tmFKxwfmPA){CXIAQBDKsg = true;}
      while(XLezspxAUK == DrwtRBPJQm){DBwoJSGdeO = true;}
      while(QOebWAPBLt == QOebWAPBLt){MtXkZKzlpH = true;}
      while(tmFKxwfmPA == tmFKxwfmPA){EkBUumGdoD = true;}
      if(GDIpGfiHsN == true){GDIpGfiHsN = false;}
      if(LcbhRZLxQG == true){LcbhRZLxQG = false;}
      if(zqsZJjFTCw == true){zqsZJjFTCw = false;}
      if(JKkTDMGnVq == true){JKkTDMGnVq = false;}
      if(QHHxhRLXrW == true){QHHxhRLXrW = false;}
      if(kiVIAOKhfV == true){kiVIAOKhfV = false;}
      if(ZPExEArHdE == true){ZPExEArHdE = false;}
      if(TWaZpqjurq == true){TWaZpqjurq = false;}
      if(YhafLaobTo == true){YhafLaobTo = false;}
      if(CXIAQBDKsg == true){CXIAQBDKsg = false;}
      if(ZEsnDOHwSI == true){ZEsnDOHwSI = false;}
      if(eOpnxrnYlc == true){eOpnxrnYlc = false;}
      if(LPnoCuOaJn == true){LPnoCuOaJn = false;}
      if(nYCKRTomcw == true){nYCKRTomcw = false;}
      if(TsIKJmhTCX == true){TsIKJmhTCX = false;}
      if(NdYPUJPEkH == true){NdYPUJPEkH = false;}
      if(AfleUGMeWc == true){AfleUGMeWc = false;}
      if(DBwoJSGdeO == true){DBwoJSGdeO = false;}
      if(MtXkZKzlpH == true){MtXkZKzlpH = false;}
      if(EkBUumGdoD == true){EkBUumGdoD = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZDVFMUNNMW
{ 
  void HMNSNMrfmi()
  { 
      bool GHppHejJsP = false;
      bool sDexbDlCOb = false;
      bool VKJeMPTxPP = false;
      bool wWRAYQqISe = false;
      bool IToEcBAboQ = false;
      bool ZAqNNuyLcr = false;
      bool KmtdSWldYK = false;
      bool LdjiTCVEBK = false;
      bool OrHyoweDlc = false;
      bool SfeZTUYLlG = false;
      bool PVCeEwPIsL = false;
      bool ZsKRdReunW = false;
      bool qirwrfyJOy = false;
      bool cKSuurpcsr = false;
      bool fWJaDYUNwB = false;
      bool pdWOOfdGlY = false;
      bool xGsuiOZOVk = false;
      bool bBCupUYQsT = false;
      bool KkNMHznDgO = false;
      bool OjqUuZdOrn = false;
      string iKqZzHFCBz;
      string oCANaBTbig;
      string KDMxFSnytq;
      string iSDJmwrHaM;
      string bmtLReVJmS;
      string YsdNkfysUg;
      string THHJPVyqad;
      string JMBWEXIAfZ;
      string wdLaSpooYG;
      string yzlsrUekww;
      string feiIJLrHJM;
      string WxNtjowfqZ;
      string gJSTfJZHdj;
      string rXEisRtCQG;
      string xgfLiAkrIC;
      string YztVOWLOcA;
      string lkdZUzsdBS;
      string usOjbzcHje;
      string XhyaKrdhTW;
      string FeYQUMDGpj;
      if(iKqZzHFCBz == feiIJLrHJM){GHppHejJsP = true;}
      else if(feiIJLrHJM == iKqZzHFCBz){PVCeEwPIsL = true;}
      if(oCANaBTbig == WxNtjowfqZ){sDexbDlCOb = true;}
      else if(WxNtjowfqZ == oCANaBTbig){ZsKRdReunW = true;}
      if(KDMxFSnytq == gJSTfJZHdj){VKJeMPTxPP = true;}
      else if(gJSTfJZHdj == KDMxFSnytq){qirwrfyJOy = true;}
      if(iSDJmwrHaM == rXEisRtCQG){wWRAYQqISe = true;}
      else if(rXEisRtCQG == iSDJmwrHaM){cKSuurpcsr = true;}
      if(bmtLReVJmS == xgfLiAkrIC){IToEcBAboQ = true;}
      else if(xgfLiAkrIC == bmtLReVJmS){fWJaDYUNwB = true;}
      if(YsdNkfysUg == YztVOWLOcA){ZAqNNuyLcr = true;}
      else if(YztVOWLOcA == YsdNkfysUg){pdWOOfdGlY = true;}
      if(THHJPVyqad == lkdZUzsdBS){KmtdSWldYK = true;}
      else if(lkdZUzsdBS == THHJPVyqad){xGsuiOZOVk = true;}
      if(JMBWEXIAfZ == usOjbzcHje){LdjiTCVEBK = true;}
      if(wdLaSpooYG == XhyaKrdhTW){OrHyoweDlc = true;}
      if(yzlsrUekww == FeYQUMDGpj){SfeZTUYLlG = true;}
      while(usOjbzcHje == JMBWEXIAfZ){bBCupUYQsT = true;}
      while(XhyaKrdhTW == XhyaKrdhTW){KkNMHznDgO = true;}
      while(FeYQUMDGpj == FeYQUMDGpj){OjqUuZdOrn = true;}
      if(GHppHejJsP == true){GHppHejJsP = false;}
      if(sDexbDlCOb == true){sDexbDlCOb = false;}
      if(VKJeMPTxPP == true){VKJeMPTxPP = false;}
      if(wWRAYQqISe == true){wWRAYQqISe = false;}
      if(IToEcBAboQ == true){IToEcBAboQ = false;}
      if(ZAqNNuyLcr == true){ZAqNNuyLcr = false;}
      if(KmtdSWldYK == true){KmtdSWldYK = false;}
      if(LdjiTCVEBK == true){LdjiTCVEBK = false;}
      if(OrHyoweDlc == true){OrHyoweDlc = false;}
      if(SfeZTUYLlG == true){SfeZTUYLlG = false;}
      if(PVCeEwPIsL == true){PVCeEwPIsL = false;}
      if(ZsKRdReunW == true){ZsKRdReunW = false;}
      if(qirwrfyJOy == true){qirwrfyJOy = false;}
      if(cKSuurpcsr == true){cKSuurpcsr = false;}
      if(fWJaDYUNwB == true){fWJaDYUNwB = false;}
      if(pdWOOfdGlY == true){pdWOOfdGlY = false;}
      if(xGsuiOZOVk == true){xGsuiOZOVk = false;}
      if(bBCupUYQsT == true){bBCupUYQsT = false;}
      if(KkNMHznDgO == true){KkNMHznDgO = false;}
      if(OjqUuZdOrn == true){OjqUuZdOrn = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class YINYYUWETN
{ 
  void QBjheuRoIu()
  { 
      bool jSBVQHFLmd = false;
      bool qVrTmCVUNT = false;
      bool poNVeMzMzT = false;
      bool PxiocqLlot = false;
      bool lnNIoQRdNM = false;
      bool kNYSstFfpE = false;
      bool HrMSOsmCqw = false;
      bool qifThBbFfN = false;
      bool uVzliBYQuh = false;
      bool kfwjlsbusu = false;
      bool dOmRVBhEkr = false;
      bool SAzbJlsZjN = false;
      bool UVHIKKmuhn = false;
      bool ZfLjjfhgQg = false;
      bool STeCOkfTaz = false;
      bool ACKfaIGdHK = false;
      bool OmWxSlnOrc = false;
      bool DLJCtAqEtY = false;
      bool eSUhnWUfJa = false;
      bool ZEZFSfwhjc = false;
      string UlqoBmVrVs;
      string PGkoBJcUAH;
      string HMoaEFzqIT;
      string WbqpzeyyjX;
      string OPUyZeJHpk;
      string JLxQRIxQju;
      string neiCWnOupV;
      string KdkbrrrqBY;
      string uqAXJKeaYp;
      string hhNnijfRal;
      string QOQUZeZUXY;
      string PGPKzwdLrL;
      string InVzncPwsI;
      string MZPPIfaexd;
      string kKCzaaSDom;
      string GTZOcbsQgx;
      string OdaPMpqzBS;
      string VyNNaykWYM;
      string JMHGBrABlf;
      string mJyAOGimmj;
      if(UlqoBmVrVs == QOQUZeZUXY){jSBVQHFLmd = true;}
      else if(QOQUZeZUXY == UlqoBmVrVs){dOmRVBhEkr = true;}
      if(PGkoBJcUAH == PGPKzwdLrL){qVrTmCVUNT = true;}
      else if(PGPKzwdLrL == PGkoBJcUAH){SAzbJlsZjN = true;}
      if(HMoaEFzqIT == InVzncPwsI){poNVeMzMzT = true;}
      else if(InVzncPwsI == HMoaEFzqIT){UVHIKKmuhn = true;}
      if(WbqpzeyyjX == MZPPIfaexd){PxiocqLlot = true;}
      else if(MZPPIfaexd == WbqpzeyyjX){ZfLjjfhgQg = true;}
      if(OPUyZeJHpk == kKCzaaSDom){lnNIoQRdNM = true;}
      else if(kKCzaaSDom == OPUyZeJHpk){STeCOkfTaz = true;}
      if(JLxQRIxQju == GTZOcbsQgx){kNYSstFfpE = true;}
      else if(GTZOcbsQgx == JLxQRIxQju){ACKfaIGdHK = true;}
      if(neiCWnOupV == OdaPMpqzBS){HrMSOsmCqw = true;}
      else if(OdaPMpqzBS == neiCWnOupV){OmWxSlnOrc = true;}
      if(KdkbrrrqBY == VyNNaykWYM){qifThBbFfN = true;}
      if(uqAXJKeaYp == JMHGBrABlf){uVzliBYQuh = true;}
      if(hhNnijfRal == mJyAOGimmj){kfwjlsbusu = true;}
      while(VyNNaykWYM == KdkbrrrqBY){DLJCtAqEtY = true;}
      while(JMHGBrABlf == JMHGBrABlf){eSUhnWUfJa = true;}
      while(mJyAOGimmj == mJyAOGimmj){ZEZFSfwhjc = true;}
      if(jSBVQHFLmd == true){jSBVQHFLmd = false;}
      if(qVrTmCVUNT == true){qVrTmCVUNT = false;}
      if(poNVeMzMzT == true){poNVeMzMzT = false;}
      if(PxiocqLlot == true){PxiocqLlot = false;}
      if(lnNIoQRdNM == true){lnNIoQRdNM = false;}
      if(kNYSstFfpE == true){kNYSstFfpE = false;}
      if(HrMSOsmCqw == true){HrMSOsmCqw = false;}
      if(qifThBbFfN == true){qifThBbFfN = false;}
      if(uVzliBYQuh == true){uVzliBYQuh = false;}
      if(kfwjlsbusu == true){kfwjlsbusu = false;}
      if(dOmRVBhEkr == true){dOmRVBhEkr = false;}
      if(SAzbJlsZjN == true){SAzbJlsZjN = false;}
      if(UVHIKKmuhn == true){UVHIKKmuhn = false;}
      if(ZfLjjfhgQg == true){ZfLjjfhgQg = false;}
      if(STeCOkfTaz == true){STeCOkfTaz = false;}
      if(ACKfaIGdHK == true){ACKfaIGdHK = false;}
      if(OmWxSlnOrc == true){OmWxSlnOrc = false;}
      if(DLJCtAqEtY == true){DLJCtAqEtY = false;}
      if(eSUhnWUfJa == true){eSUhnWUfJa = false;}
      if(ZEZFSfwhjc == true){ZEZFSfwhjc = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QZITOAXIVW
{ 
  void lSBqtPHJMZ()
  { 
      bool ybYssWswRy = false;
      bool UucfEowFuT = false;
      bool EWzfgntbUz = false;
      bool BtTYKNLqba = false;
      bool fBTKzclfAp = false;
      bool AchbIacsLE = false;
      bool bRbSwaaDUq = false;
      bool cFdTnCDnfD = false;
      bool CObdyYLrPj = false;
      bool jReXzOYBNY = false;
      bool BiKJMuzNnG = false;
      bool XnNHCMpyTX = false;
      bool OimkGLaGUZ = false;
      bool dBaGCSDRRD = false;
      bool YyDJLuEWUl = false;
      bool YagpKRwZCB = false;
      bool TrsNcVNGFi = false;
      bool JXfQyQecYL = false;
      bool GEkiBCUjwj = false;
      bool WcPPigEpgp = false;
      string qKVatPOCPw;
      string mDWdKXrwHK;
      string MuFxQFjzCV;
      string bpUhLYKwaK;
      string zGxLaTFlrh;
      string fioezwFXJn;
      string fOztUwinSk;
      string NwUwLGxsWx;
      string AxHTorBoiO;
      string TlllejGCqd;
      string kQSGUjOrTs;
      string yLBMnhHfFb;
      string tsFDRaJhRa;
      string zuCrMieAyF;
      string dJVoQHkYyw;
      string cZxPirLPot;
      string ZfarsCJIAF;
      string DGgAxkWPmJ;
      string pjjUxYylJU;
      string XOLEexicwN;
      if(qKVatPOCPw == kQSGUjOrTs){ybYssWswRy = true;}
      else if(kQSGUjOrTs == qKVatPOCPw){BiKJMuzNnG = true;}
      if(mDWdKXrwHK == yLBMnhHfFb){UucfEowFuT = true;}
      else if(yLBMnhHfFb == mDWdKXrwHK){XnNHCMpyTX = true;}
      if(MuFxQFjzCV == tsFDRaJhRa){EWzfgntbUz = true;}
      else if(tsFDRaJhRa == MuFxQFjzCV){OimkGLaGUZ = true;}
      if(bpUhLYKwaK == zuCrMieAyF){BtTYKNLqba = true;}
      else if(zuCrMieAyF == bpUhLYKwaK){dBaGCSDRRD = true;}
      if(zGxLaTFlrh == dJVoQHkYyw){fBTKzclfAp = true;}
      else if(dJVoQHkYyw == zGxLaTFlrh){YyDJLuEWUl = true;}
      if(fioezwFXJn == cZxPirLPot){AchbIacsLE = true;}
      else if(cZxPirLPot == fioezwFXJn){YagpKRwZCB = true;}
      if(fOztUwinSk == ZfarsCJIAF){bRbSwaaDUq = true;}
      else if(ZfarsCJIAF == fOztUwinSk){TrsNcVNGFi = true;}
      if(NwUwLGxsWx == DGgAxkWPmJ){cFdTnCDnfD = true;}
      if(AxHTorBoiO == pjjUxYylJU){CObdyYLrPj = true;}
      if(TlllejGCqd == XOLEexicwN){jReXzOYBNY = true;}
      while(DGgAxkWPmJ == NwUwLGxsWx){JXfQyQecYL = true;}
      while(pjjUxYylJU == pjjUxYylJU){GEkiBCUjwj = true;}
      while(XOLEexicwN == XOLEexicwN){WcPPigEpgp = true;}
      if(ybYssWswRy == true){ybYssWswRy = false;}
      if(UucfEowFuT == true){UucfEowFuT = false;}
      if(EWzfgntbUz == true){EWzfgntbUz = false;}
      if(BtTYKNLqba == true){BtTYKNLqba = false;}
      if(fBTKzclfAp == true){fBTKzclfAp = false;}
      if(AchbIacsLE == true){AchbIacsLE = false;}
      if(bRbSwaaDUq == true){bRbSwaaDUq = false;}
      if(cFdTnCDnfD == true){cFdTnCDnfD = false;}
      if(CObdyYLrPj == true){CObdyYLrPj = false;}
      if(jReXzOYBNY == true){jReXzOYBNY = false;}
      if(BiKJMuzNnG == true){BiKJMuzNnG = false;}
      if(XnNHCMpyTX == true){XnNHCMpyTX = false;}
      if(OimkGLaGUZ == true){OimkGLaGUZ = false;}
      if(dBaGCSDRRD == true){dBaGCSDRRD = false;}
      if(YyDJLuEWUl == true){YyDJLuEWUl = false;}
      if(YagpKRwZCB == true){YagpKRwZCB = false;}
      if(TrsNcVNGFi == true){TrsNcVNGFi = false;}
      if(JXfQyQecYL == true){JXfQyQecYL = false;}
      if(GEkiBCUjwj == true){GEkiBCUjwj = false;}
      if(WcPPigEpgp == true){WcPPigEpgp = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NFBTFNWMII
{ 
  void VCfEalmCSj()
  { 
      bool JTbnpffyRd = false;
      bool hjDilqmGlR = false;
      bool SaeJssZImg = false;
      bool JopNGNHXxP = false;
      bool fSWtQoKXJj = false;
      bool mboZTinStw = false;
      bool FXLkUsspbY = false;
      bool iOJVkzDjrD = false;
      bool iCzCshzQtX = false;
      bool YocAKIHlZW = false;
      bool wUujAXRSnT = false;
      bool fEZqNCYIqm = false;
      bool sbwDAtlILc = false;
      bool qQKVGNGOCV = false;
      bool PgcZrTPlmH = false;
      bool jzumdUXrjG = false;
      bool lIGLbOtDjz = false;
      bool uJGVWMzVLf = false;
      bool ezVErrVWrl = false;
      bool oOdwiGZQxc = false;
      string RyBmOqYAER;
      string oPbNCpVbIs;
      string KuzNNoXmrt;
      string sHsYGFMOqt;
      string kKmdZsZfoD;
      string ACwwaVznZZ;
      string TlHKXgVCGx;
      string dHjNHzHUnU;
      string OryntqLxxj;
      string FXoRazDcGr;
      string WWYqYrRhoa;
      string OeAFKbmSPg;
      string VYEVqFSSmH;
      string wdOuxqSGOB;
      string YcIWVZXiGz;
      string MjMYWyRDAT;
      string ZztgejPEHS;
      string IgDOHrPhzQ;
      string giLnuMRoYy;
      string ZOGuaZhaql;
      if(RyBmOqYAER == WWYqYrRhoa){JTbnpffyRd = true;}
      else if(WWYqYrRhoa == RyBmOqYAER){wUujAXRSnT = true;}
      if(oPbNCpVbIs == OeAFKbmSPg){hjDilqmGlR = true;}
      else if(OeAFKbmSPg == oPbNCpVbIs){fEZqNCYIqm = true;}
      if(KuzNNoXmrt == VYEVqFSSmH){SaeJssZImg = true;}
      else if(VYEVqFSSmH == KuzNNoXmrt){sbwDAtlILc = true;}
      if(sHsYGFMOqt == wdOuxqSGOB){JopNGNHXxP = true;}
      else if(wdOuxqSGOB == sHsYGFMOqt){qQKVGNGOCV = true;}
      if(kKmdZsZfoD == YcIWVZXiGz){fSWtQoKXJj = true;}
      else if(YcIWVZXiGz == kKmdZsZfoD){PgcZrTPlmH = true;}
      if(ACwwaVznZZ == MjMYWyRDAT){mboZTinStw = true;}
      else if(MjMYWyRDAT == ACwwaVznZZ){jzumdUXrjG = true;}
      if(TlHKXgVCGx == ZztgejPEHS){FXLkUsspbY = true;}
      else if(ZztgejPEHS == TlHKXgVCGx){lIGLbOtDjz = true;}
      if(dHjNHzHUnU == IgDOHrPhzQ){iOJVkzDjrD = true;}
      if(OryntqLxxj == giLnuMRoYy){iCzCshzQtX = true;}
      if(FXoRazDcGr == ZOGuaZhaql){YocAKIHlZW = true;}
      while(IgDOHrPhzQ == dHjNHzHUnU){uJGVWMzVLf = true;}
      while(giLnuMRoYy == giLnuMRoYy){ezVErrVWrl = true;}
      while(ZOGuaZhaql == ZOGuaZhaql){oOdwiGZQxc = true;}
      if(JTbnpffyRd == true){JTbnpffyRd = false;}
      if(hjDilqmGlR == true){hjDilqmGlR = false;}
      if(SaeJssZImg == true){SaeJssZImg = false;}
      if(JopNGNHXxP == true){JopNGNHXxP = false;}
      if(fSWtQoKXJj == true){fSWtQoKXJj = false;}
      if(mboZTinStw == true){mboZTinStw = false;}
      if(FXLkUsspbY == true){FXLkUsspbY = false;}
      if(iOJVkzDjrD == true){iOJVkzDjrD = false;}
      if(iCzCshzQtX == true){iCzCshzQtX = false;}
      if(YocAKIHlZW == true){YocAKIHlZW = false;}
      if(wUujAXRSnT == true){wUujAXRSnT = false;}
      if(fEZqNCYIqm == true){fEZqNCYIqm = false;}
      if(sbwDAtlILc == true){sbwDAtlILc = false;}
      if(qQKVGNGOCV == true){qQKVGNGOCV = false;}
      if(PgcZrTPlmH == true){PgcZrTPlmH = false;}
      if(jzumdUXrjG == true){jzumdUXrjG = false;}
      if(lIGLbOtDjz == true){lIGLbOtDjz = false;}
      if(uJGVWMzVLf == true){uJGVWMzVLf = false;}
      if(ezVErrVWrl == true){ezVErrVWrl = false;}
      if(oOdwiGZQxc == true){oOdwiGZQxc = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KLMKWFYKFV
{ 
  void ffEtuHaosB()
  { 
      bool JJFChxbBpl = false;
      bool GnIPkEhfwM = false;
      bool ToYJHbepLq = false;
      bool OGLeKLzuoX = false;
      bool ILPHDIPrJu = false;
      bool PpaukUkwuF = false;
      bool SkpyXAcdEY = false;
      bool FlpYEHYRFy = false;
      bool MmgPaYfPsp = false;
      bool aCAiFWVQiD = false;
      bool flmjoczBpd = false;
      bool uTqwsfEZWz = false;
      bool UMkOyClGSh = false;
      bool lHhBSZzway = false;
      bool YdZOtGLDoZ = false;
      bool SusjhSBYll = false;
      bool priIqmlodT = false;
      bool fHzYZqmVTq = false;
      bool qhQsOpNxTz = false;
      bool XaKnHfuQWG = false;
      string jiKzmSGNXO;
      string ylPnJEWjGQ;
      string oOTbkrTXUE;
      string cCFpSztEMz;
      string xrDjckFlUP;
      string GKRrqUOhOu;
      string yhrTlugYVK;
      string KSWHTwLiyz;
      string lgDFxRkNzx;
      string jLoQAOzjyh;
      string EgQKEmmKXt;
      string JfiPJRelVT;
      string KTYXZydGGF;
      string xbkQuEQrBJ;
      string eZoFnEcYHi;
      string jMNeWjHrkG;
      string JgDRxhuSTE;
      string AlkLojdTjU;
      string VEyXVjNFsK;
      string xXHtAxVWIF;
      if(jiKzmSGNXO == EgQKEmmKXt){JJFChxbBpl = true;}
      else if(EgQKEmmKXt == jiKzmSGNXO){flmjoczBpd = true;}
      if(ylPnJEWjGQ == JfiPJRelVT){GnIPkEhfwM = true;}
      else if(JfiPJRelVT == ylPnJEWjGQ){uTqwsfEZWz = true;}
      if(oOTbkrTXUE == KTYXZydGGF){ToYJHbepLq = true;}
      else if(KTYXZydGGF == oOTbkrTXUE){UMkOyClGSh = true;}
      if(cCFpSztEMz == xbkQuEQrBJ){OGLeKLzuoX = true;}
      else if(xbkQuEQrBJ == cCFpSztEMz){lHhBSZzway = true;}
      if(xrDjckFlUP == eZoFnEcYHi){ILPHDIPrJu = true;}
      else if(eZoFnEcYHi == xrDjckFlUP){YdZOtGLDoZ = true;}
      if(GKRrqUOhOu == jMNeWjHrkG){PpaukUkwuF = true;}
      else if(jMNeWjHrkG == GKRrqUOhOu){SusjhSBYll = true;}
      if(yhrTlugYVK == JgDRxhuSTE){SkpyXAcdEY = true;}
      else if(JgDRxhuSTE == yhrTlugYVK){priIqmlodT = true;}
      if(KSWHTwLiyz == AlkLojdTjU){FlpYEHYRFy = true;}
      if(lgDFxRkNzx == VEyXVjNFsK){MmgPaYfPsp = true;}
      if(jLoQAOzjyh == xXHtAxVWIF){aCAiFWVQiD = true;}
      while(AlkLojdTjU == KSWHTwLiyz){fHzYZqmVTq = true;}
      while(VEyXVjNFsK == VEyXVjNFsK){qhQsOpNxTz = true;}
      while(xXHtAxVWIF == xXHtAxVWIF){XaKnHfuQWG = true;}
      if(JJFChxbBpl == true){JJFChxbBpl = false;}
      if(GnIPkEhfwM == true){GnIPkEhfwM = false;}
      if(ToYJHbepLq == true){ToYJHbepLq = false;}
      if(OGLeKLzuoX == true){OGLeKLzuoX = false;}
      if(ILPHDIPrJu == true){ILPHDIPrJu = false;}
      if(PpaukUkwuF == true){PpaukUkwuF = false;}
      if(SkpyXAcdEY == true){SkpyXAcdEY = false;}
      if(FlpYEHYRFy == true){FlpYEHYRFy = false;}
      if(MmgPaYfPsp == true){MmgPaYfPsp = false;}
      if(aCAiFWVQiD == true){aCAiFWVQiD = false;}
      if(flmjoczBpd == true){flmjoczBpd = false;}
      if(uTqwsfEZWz == true){uTqwsfEZWz = false;}
      if(UMkOyClGSh == true){UMkOyClGSh = false;}
      if(lHhBSZzway == true){lHhBSZzway = false;}
      if(YdZOtGLDoZ == true){YdZOtGLDoZ = false;}
      if(SusjhSBYll == true){SusjhSBYll = false;}
      if(priIqmlodT == true){priIqmlodT = false;}
      if(fHzYZqmVTq == true){fHzYZqmVTq = false;}
      if(qhQsOpNxTz == true){qhQsOpNxTz = false;}
      if(XaKnHfuQWG == true){XaKnHfuQWG = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AFBJUCQFAF
{ 
  void HIrujJUcYr()
  { 
      bool cRUJScYGlL = false;
      bool GoBtMQPaXT = false;
      bool efmrBBpCrk = false;
      bool BLYCKGRqpa = false;
      bool oiIkzwshHx = false;
      bool fKikjxdcHQ = false;
      bool lWNBKuQzjF = false;
      bool zYjxduWCsE = false;
      bool HHtuEpbmRN = false;
      bool boiYnRVHZE = false;
      bool ftyCyHakGo = false;
      bool ZbiCJdcSwT = false;
      bool ScgWdJoKIL = false;
      bool ZPcnYNJrwx = false;
      bool PqgcmAFgPJ = false;
      bool oCKFkDMbYn = false;
      bool RPbuPKzudw = false;
      bool UkVzEawEic = false;
      bool EHtzWWEWCU = false;
      bool AgTIezOkMb = false;
      string zfjXyISkmy;
      string TaFnBgbIMU;
      string AgMzqSPAqk;
      string bgOIbNUbUQ;
      string sRojtlfgNX;
      string JBDDbRSkge;
      string rRtBVlMxLm;
      string LAmwbircEE;
      string milSbacKgJ;
      string tjuoXbXnNI;
      string ZiUaMJNHbs;
      string ZpgbrJebZt;
      string IBPCrbFxXA;
      string KEOwdENLFS;
      string MVimjECSxO;
      string wuNTzYSShk;
      string DQjbhUajxn;
      string wTVHgmScRn;
      string YlAJDOpuxF;
      string ltcTOwlHEP;
      if(zfjXyISkmy == ZiUaMJNHbs){cRUJScYGlL = true;}
      else if(ZiUaMJNHbs == zfjXyISkmy){ftyCyHakGo = true;}
      if(TaFnBgbIMU == ZpgbrJebZt){GoBtMQPaXT = true;}
      else if(ZpgbrJebZt == TaFnBgbIMU){ZbiCJdcSwT = true;}
      if(AgMzqSPAqk == IBPCrbFxXA){efmrBBpCrk = true;}
      else if(IBPCrbFxXA == AgMzqSPAqk){ScgWdJoKIL = true;}
      if(bgOIbNUbUQ == KEOwdENLFS){BLYCKGRqpa = true;}
      else if(KEOwdENLFS == bgOIbNUbUQ){ZPcnYNJrwx = true;}
      if(sRojtlfgNX == MVimjECSxO){oiIkzwshHx = true;}
      else if(MVimjECSxO == sRojtlfgNX){PqgcmAFgPJ = true;}
      if(JBDDbRSkge == wuNTzYSShk){fKikjxdcHQ = true;}
      else if(wuNTzYSShk == JBDDbRSkge){oCKFkDMbYn = true;}
      if(rRtBVlMxLm == DQjbhUajxn){lWNBKuQzjF = true;}
      else if(DQjbhUajxn == rRtBVlMxLm){RPbuPKzudw = true;}
      if(LAmwbircEE == wTVHgmScRn){zYjxduWCsE = true;}
      if(milSbacKgJ == YlAJDOpuxF){HHtuEpbmRN = true;}
      if(tjuoXbXnNI == ltcTOwlHEP){boiYnRVHZE = true;}
      while(wTVHgmScRn == LAmwbircEE){UkVzEawEic = true;}
      while(YlAJDOpuxF == YlAJDOpuxF){EHtzWWEWCU = true;}
      while(ltcTOwlHEP == ltcTOwlHEP){AgTIezOkMb = true;}
      if(cRUJScYGlL == true){cRUJScYGlL = false;}
      if(GoBtMQPaXT == true){GoBtMQPaXT = false;}
      if(efmrBBpCrk == true){efmrBBpCrk = false;}
      if(BLYCKGRqpa == true){BLYCKGRqpa = false;}
      if(oiIkzwshHx == true){oiIkzwshHx = false;}
      if(fKikjxdcHQ == true){fKikjxdcHQ = false;}
      if(lWNBKuQzjF == true){lWNBKuQzjF = false;}
      if(zYjxduWCsE == true){zYjxduWCsE = false;}
      if(HHtuEpbmRN == true){HHtuEpbmRN = false;}
      if(boiYnRVHZE == true){boiYnRVHZE = false;}
      if(ftyCyHakGo == true){ftyCyHakGo = false;}
      if(ZbiCJdcSwT == true){ZbiCJdcSwT = false;}
      if(ScgWdJoKIL == true){ScgWdJoKIL = false;}
      if(ZPcnYNJrwx == true){ZPcnYNJrwx = false;}
      if(PqgcmAFgPJ == true){PqgcmAFgPJ = false;}
      if(oCKFkDMbYn == true){oCKFkDMbYn = false;}
      if(RPbuPKzudw == true){RPbuPKzudw = false;}
      if(UkVzEawEic == true){UkVzEawEic = false;}
      if(EHtzWWEWCU == true){EHtzWWEWCU = false;}
      if(AgTIezOkMb == true){AgTIezOkMb = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class NFXEBWLYIE
{ 
  void wcjQqskGqB()
  { 
      bool yQYALSMhXz = false;
      bool pnuOdDLJYl = false;
      bool MnVkCLrhiA = false;
      bool gtbDTcIZRr = false;
      bool xjwzwbjJqP = false;
      bool rdppEKtnxz = false;
      bool adyWSwMrtA = false;
      bool tlqfobrVoa = false;
      bool biqSICDkzJ = false;
      bool iUcSRTkYgA = false;
      bool PrOpUGlKBc = false;
      bool RYzWWqmfOQ = false;
      bool rarkRZJYOU = false;
      bool pTonLjTAxB = false;
      bool QsjWqKVGwZ = false;
      bool fFJPHnzdXu = false;
      bool xNuflyiKLl = false;
      bool XlhMAzqVsH = false;
      bool EDbnojJYgW = false;
      bool kpbCECxEjr = false;
      string nyVwiSuHyL;
      string ewcxhOAsds;
      string sMStpaCKMW;
      string rmKDEsFNDG;
      string qGStItuZwO;
      string CUqfIaebFH;
      string PktbTrlYsn;
      string JloWBltYku;
      string ebNyUWuduc;
      string OcHupGMqZQ;
      string IVgHEPfTeV;
      string gGpKbmWwGw;
      string wIdQZgesfu;
      string ZCyUmIuFsE;
      string qbzYuHlwTD;
      string jPjdUArPhX;
      string klmjweIVXS;
      string qQDCFUJEUS;
      string thZcLJSTzm;
      string QeXUTEnOcQ;
      if(nyVwiSuHyL == IVgHEPfTeV){yQYALSMhXz = true;}
      else if(IVgHEPfTeV == nyVwiSuHyL){PrOpUGlKBc = true;}
      if(ewcxhOAsds == gGpKbmWwGw){pnuOdDLJYl = true;}
      else if(gGpKbmWwGw == ewcxhOAsds){RYzWWqmfOQ = true;}
      if(sMStpaCKMW == wIdQZgesfu){MnVkCLrhiA = true;}
      else if(wIdQZgesfu == sMStpaCKMW){rarkRZJYOU = true;}
      if(rmKDEsFNDG == ZCyUmIuFsE){gtbDTcIZRr = true;}
      else if(ZCyUmIuFsE == rmKDEsFNDG){pTonLjTAxB = true;}
      if(qGStItuZwO == qbzYuHlwTD){xjwzwbjJqP = true;}
      else if(qbzYuHlwTD == qGStItuZwO){QsjWqKVGwZ = true;}
      if(CUqfIaebFH == jPjdUArPhX){rdppEKtnxz = true;}
      else if(jPjdUArPhX == CUqfIaebFH){fFJPHnzdXu = true;}
      if(PktbTrlYsn == klmjweIVXS){adyWSwMrtA = true;}
      else if(klmjweIVXS == PktbTrlYsn){xNuflyiKLl = true;}
      if(JloWBltYku == qQDCFUJEUS){tlqfobrVoa = true;}
      if(ebNyUWuduc == thZcLJSTzm){biqSICDkzJ = true;}
      if(OcHupGMqZQ == QeXUTEnOcQ){iUcSRTkYgA = true;}
      while(qQDCFUJEUS == JloWBltYku){XlhMAzqVsH = true;}
      while(thZcLJSTzm == thZcLJSTzm){EDbnojJYgW = true;}
      while(QeXUTEnOcQ == QeXUTEnOcQ){kpbCECxEjr = true;}
      if(yQYALSMhXz == true){yQYALSMhXz = false;}
      if(pnuOdDLJYl == true){pnuOdDLJYl = false;}
      if(MnVkCLrhiA == true){MnVkCLrhiA = false;}
      if(gtbDTcIZRr == true){gtbDTcIZRr = false;}
      if(xjwzwbjJqP == true){xjwzwbjJqP = false;}
      if(rdppEKtnxz == true){rdppEKtnxz = false;}
      if(adyWSwMrtA == true){adyWSwMrtA = false;}
      if(tlqfobrVoa == true){tlqfobrVoa = false;}
      if(biqSICDkzJ == true){biqSICDkzJ = false;}
      if(iUcSRTkYgA == true){iUcSRTkYgA = false;}
      if(PrOpUGlKBc == true){PrOpUGlKBc = false;}
      if(RYzWWqmfOQ == true){RYzWWqmfOQ = false;}
      if(rarkRZJYOU == true){rarkRZJYOU = false;}
      if(pTonLjTAxB == true){pTonLjTAxB = false;}
      if(QsjWqKVGwZ == true){QsjWqKVGwZ = false;}
      if(fFJPHnzdXu == true){fFJPHnzdXu = false;}
      if(xNuflyiKLl == true){xNuflyiKLl = false;}
      if(XlhMAzqVsH == true){XlhMAzqVsH = false;}
      if(EDbnojJYgW == true){EDbnojJYgW = false;}
      if(kpbCECxEjr == true){kpbCECxEjr = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZLLSTRQHWT
{ 
  void umyBPMateW()
  { 
      bool xZnVMkUKDC = false;
      bool YFDFHnrLXx = false;
      bool NaIUVTZdIU = false;
      bool eEwshZTmPX = false;
      bool jGISFWqmyd = false;
      bool PKfAEVRSAs = false;
      bool LqBiXTJJXg = false;
      bool UEuzNHSpPz = false;
      bool cleeFTYwoL = false;
      bool IAmWMsEZtk = false;
      bool NhPRIKPfmw = false;
      bool DbDAPYLWcD = false;
      bool uNTMXURXmU = false;
      bool DsudluWMVu = false;
      bool ErIsHfwlGn = false;
      bool ggwqpzNDsn = false;
      bool rqsSsKgZOU = false;
      bool ByAhzCEaEr = false;
      bool XfQymVwDnl = false;
      bool gTThCMzpzx = false;
      string OrIiUInTdT;
      string RVjjpFfWMq;
      string hBBSoVANwi;
      string uCsesIupuH;
      string bXWpeDyQiW;
      string eGyLUNqoty;
      string IaWtYPbEph;
      string KoaEOJeKmk;
      string WjeZEnDhKf;
      string RFuxzABbRO;
      string atZBMoeswK;
      string hfBJbXwxpk;
      string LttKPEsdEO;
      string ICsYKcJdNO;
      string IKlzxkDlUa;
      string EElfDgAPkH;
      string wBnhBemcyy;
      string OSiacAItMe;
      string AHJcJEQMUd;
      string ctrFSHVDxS;
      if(OrIiUInTdT == atZBMoeswK){xZnVMkUKDC = true;}
      else if(atZBMoeswK == OrIiUInTdT){NhPRIKPfmw = true;}
      if(RVjjpFfWMq == hfBJbXwxpk){YFDFHnrLXx = true;}
      else if(hfBJbXwxpk == RVjjpFfWMq){DbDAPYLWcD = true;}
      if(hBBSoVANwi == LttKPEsdEO){NaIUVTZdIU = true;}
      else if(LttKPEsdEO == hBBSoVANwi){uNTMXURXmU = true;}
      if(uCsesIupuH == ICsYKcJdNO){eEwshZTmPX = true;}
      else if(ICsYKcJdNO == uCsesIupuH){DsudluWMVu = true;}
      if(bXWpeDyQiW == IKlzxkDlUa){jGISFWqmyd = true;}
      else if(IKlzxkDlUa == bXWpeDyQiW){ErIsHfwlGn = true;}
      if(eGyLUNqoty == EElfDgAPkH){PKfAEVRSAs = true;}
      else if(EElfDgAPkH == eGyLUNqoty){ggwqpzNDsn = true;}
      if(IaWtYPbEph == wBnhBemcyy){LqBiXTJJXg = true;}
      else if(wBnhBemcyy == IaWtYPbEph){rqsSsKgZOU = true;}
      if(KoaEOJeKmk == OSiacAItMe){UEuzNHSpPz = true;}
      if(WjeZEnDhKf == AHJcJEQMUd){cleeFTYwoL = true;}
      if(RFuxzABbRO == ctrFSHVDxS){IAmWMsEZtk = true;}
      while(OSiacAItMe == KoaEOJeKmk){ByAhzCEaEr = true;}
      while(AHJcJEQMUd == AHJcJEQMUd){XfQymVwDnl = true;}
      while(ctrFSHVDxS == ctrFSHVDxS){gTThCMzpzx = true;}
      if(xZnVMkUKDC == true){xZnVMkUKDC = false;}
      if(YFDFHnrLXx == true){YFDFHnrLXx = false;}
      if(NaIUVTZdIU == true){NaIUVTZdIU = false;}
      if(eEwshZTmPX == true){eEwshZTmPX = false;}
      if(jGISFWqmyd == true){jGISFWqmyd = false;}
      if(PKfAEVRSAs == true){PKfAEVRSAs = false;}
      if(LqBiXTJJXg == true){LqBiXTJJXg = false;}
      if(UEuzNHSpPz == true){UEuzNHSpPz = false;}
      if(cleeFTYwoL == true){cleeFTYwoL = false;}
      if(IAmWMsEZtk == true){IAmWMsEZtk = false;}
      if(NhPRIKPfmw == true){NhPRIKPfmw = false;}
      if(DbDAPYLWcD == true){DbDAPYLWcD = false;}
      if(uNTMXURXmU == true){uNTMXURXmU = false;}
      if(DsudluWMVu == true){DsudluWMVu = false;}
      if(ErIsHfwlGn == true){ErIsHfwlGn = false;}
      if(ggwqpzNDsn == true){ggwqpzNDsn = false;}
      if(rqsSsKgZOU == true){rqsSsKgZOU = false;}
      if(ByAhzCEaEr == true){ByAhzCEaEr = false;}
      if(XfQymVwDnl == true){XfQymVwDnl = false;}
      if(gTThCMzpzx == true){gTThCMzpzx = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class MKVQPJTSCO
{ 
  void RIWbVmyMRc()
  { 
      bool EgCSMLCqNV = false;
      bool YoUXogHiHO = false;
      bool AahdeYyIEw = false;
      bool TkCJcGpYbW = false;
      bool LfMVrXqZrh = false;
      bool NGXBZdzYnm = false;
      bool MsmQCnlKti = false;
      bool phHjDUNLkG = false;
      bool hYQViNNBjz = false;
      bool HQjnKRrwTE = false;
      bool kfeJrezmxF = false;
      bool QmngEmcRho = false;
      bool akauaYxSYk = false;
      bool bRrgVBtHdE = false;
      bool OZDdiUcDjW = false;
      bool oaOedAFiiS = false;
      bool ggOMdzuHLC = false;
      bool sVmcaAhwMG = false;
      bool wthBuADMCU = false;
      bool wKYLXgLpAj = false;
      string wcmmDsrBiL;
      string FRODxeiCsC;
      string omxQzBjuNS;
      string DOeVOtcoZI;
      string EkjZqPMdxb;
      string LfscpJfWmB;
      string IxnPIpmrnD;
      string BsbkxECXYj;
      string oGxlHXUBfF;
      string OFCYzHwTQA;
      string bTotZRDIfl;
      string GIyDMCzCIk;
      string VlMinwqDkp;
      string IiGXHURDLo;
      string TMUtyGHyLx;
      string HsKeepHFFd;
      string ZzWpdXBZur;
      string kwqTRIQSkh;
      string nsrnhOXarR;
      string MLxhOUoePa;
      if(wcmmDsrBiL == bTotZRDIfl){EgCSMLCqNV = true;}
      else if(bTotZRDIfl == wcmmDsrBiL){kfeJrezmxF = true;}
      if(FRODxeiCsC == GIyDMCzCIk){YoUXogHiHO = true;}
      else if(GIyDMCzCIk == FRODxeiCsC){QmngEmcRho = true;}
      if(omxQzBjuNS == VlMinwqDkp){AahdeYyIEw = true;}
      else if(VlMinwqDkp == omxQzBjuNS){akauaYxSYk = true;}
      if(DOeVOtcoZI == IiGXHURDLo){TkCJcGpYbW = true;}
      else if(IiGXHURDLo == DOeVOtcoZI){bRrgVBtHdE = true;}
      if(EkjZqPMdxb == TMUtyGHyLx){LfMVrXqZrh = true;}
      else if(TMUtyGHyLx == EkjZqPMdxb){OZDdiUcDjW = true;}
      if(LfscpJfWmB == HsKeepHFFd){NGXBZdzYnm = true;}
      else if(HsKeepHFFd == LfscpJfWmB){oaOedAFiiS = true;}
      if(IxnPIpmrnD == ZzWpdXBZur){MsmQCnlKti = true;}
      else if(ZzWpdXBZur == IxnPIpmrnD){ggOMdzuHLC = true;}
      if(BsbkxECXYj == kwqTRIQSkh){phHjDUNLkG = true;}
      if(oGxlHXUBfF == nsrnhOXarR){hYQViNNBjz = true;}
      if(OFCYzHwTQA == MLxhOUoePa){HQjnKRrwTE = true;}
      while(kwqTRIQSkh == BsbkxECXYj){sVmcaAhwMG = true;}
      while(nsrnhOXarR == nsrnhOXarR){wthBuADMCU = true;}
      while(MLxhOUoePa == MLxhOUoePa){wKYLXgLpAj = true;}
      if(EgCSMLCqNV == true){EgCSMLCqNV = false;}
      if(YoUXogHiHO == true){YoUXogHiHO = false;}
      if(AahdeYyIEw == true){AahdeYyIEw = false;}
      if(TkCJcGpYbW == true){TkCJcGpYbW = false;}
      if(LfMVrXqZrh == true){LfMVrXqZrh = false;}
      if(NGXBZdzYnm == true){NGXBZdzYnm = false;}
      if(MsmQCnlKti == true){MsmQCnlKti = false;}
      if(phHjDUNLkG == true){phHjDUNLkG = false;}
      if(hYQViNNBjz == true){hYQViNNBjz = false;}
      if(HQjnKRrwTE == true){HQjnKRrwTE = false;}
      if(kfeJrezmxF == true){kfeJrezmxF = false;}
      if(QmngEmcRho == true){QmngEmcRho = false;}
      if(akauaYxSYk == true){akauaYxSYk = false;}
      if(bRrgVBtHdE == true){bRrgVBtHdE = false;}
      if(OZDdiUcDjW == true){OZDdiUcDjW = false;}
      if(oaOedAFiiS == true){oaOedAFiiS = false;}
      if(ggOMdzuHLC == true){ggOMdzuHLC = false;}
      if(sVmcaAhwMG == true){sVmcaAhwMG = false;}
      if(wthBuADMCU == true){wthBuADMCU = false;}
      if(wKYLXgLpAj == true){wKYLXgLpAj = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CGQXNTANAP
{ 
  void cNmhrnRRah()
  { 
      bool IfquqsnBGg = false;
      bool ybiCNFBfgY = false;
      bool YjBTnQItoa = false;
      bool uhZAcDKHLm = false;
      bool GxClbgFnHs = false;
      bool NiKxSYeWGZ = false;
      bool ktLfkQBhZK = false;
      bool usUtRahOrH = false;
      bool LUmuMMWbhE = false;
      bool YmBkBMkVBK = false;
      bool eWNxxUOQfr = false;
      bool pQdyOQHQTY = false;
      bool sZAUlHqirE = false;
      bool oLMhbnGoxr = false;
      bool xGwpNIJCVk = false;
      bool FUCcRFWsLp = false;
      bool ceXaCeDwWa = false;
      bool JRSbblcJGc = false;
      bool ynNGzTJjPV = false;
      bool QMdHORhVmi = false;
      string kRGYaoYbDV;
      string aHQtMtoSIN;
      string GBBKdCShuJ;
      string pihSLTdXyo;
      string OScmtcJSXx;
      string JVBmHjPwtY;
      string VpfseqZyis;
      string QQUSzmXOPB;
      string sAKyzUgtKa;
      string DgdYSYmjHC;
      string DLiefsAinU;
      string LAgrfVehtK;
      string KemHoilwzZ;
      string AnGMyhQfRF;
      string TyFpOCAksx;
      string sEoxrXXbow;
      string GEiNTXtUty;
      string FzPnpLYwrl;
      string iBCCguzJTZ;
      string JnpVzPPqVq;
      if(kRGYaoYbDV == DLiefsAinU){IfquqsnBGg = true;}
      else if(DLiefsAinU == kRGYaoYbDV){eWNxxUOQfr = true;}
      if(aHQtMtoSIN == LAgrfVehtK){ybiCNFBfgY = true;}
      else if(LAgrfVehtK == aHQtMtoSIN){pQdyOQHQTY = true;}
      if(GBBKdCShuJ == KemHoilwzZ){YjBTnQItoa = true;}
      else if(KemHoilwzZ == GBBKdCShuJ){sZAUlHqirE = true;}
      if(pihSLTdXyo == AnGMyhQfRF){uhZAcDKHLm = true;}
      else if(AnGMyhQfRF == pihSLTdXyo){oLMhbnGoxr = true;}
      if(OScmtcJSXx == TyFpOCAksx){GxClbgFnHs = true;}
      else if(TyFpOCAksx == OScmtcJSXx){xGwpNIJCVk = true;}
      if(JVBmHjPwtY == sEoxrXXbow){NiKxSYeWGZ = true;}
      else if(sEoxrXXbow == JVBmHjPwtY){FUCcRFWsLp = true;}
      if(VpfseqZyis == GEiNTXtUty){ktLfkQBhZK = true;}
      else if(GEiNTXtUty == VpfseqZyis){ceXaCeDwWa = true;}
      if(QQUSzmXOPB == FzPnpLYwrl){usUtRahOrH = true;}
      if(sAKyzUgtKa == iBCCguzJTZ){LUmuMMWbhE = true;}
      if(DgdYSYmjHC == JnpVzPPqVq){YmBkBMkVBK = true;}
      while(FzPnpLYwrl == QQUSzmXOPB){JRSbblcJGc = true;}
      while(iBCCguzJTZ == iBCCguzJTZ){ynNGzTJjPV = true;}
      while(JnpVzPPqVq == JnpVzPPqVq){QMdHORhVmi = true;}
      if(IfquqsnBGg == true){IfquqsnBGg = false;}
      if(ybiCNFBfgY == true){ybiCNFBfgY = false;}
      if(YjBTnQItoa == true){YjBTnQItoa = false;}
      if(uhZAcDKHLm == true){uhZAcDKHLm = false;}
      if(GxClbgFnHs == true){GxClbgFnHs = false;}
      if(NiKxSYeWGZ == true){NiKxSYeWGZ = false;}
      if(ktLfkQBhZK == true){ktLfkQBhZK = false;}
      if(usUtRahOrH == true){usUtRahOrH = false;}
      if(LUmuMMWbhE == true){LUmuMMWbhE = false;}
      if(YmBkBMkVBK == true){YmBkBMkVBK = false;}
      if(eWNxxUOQfr == true){eWNxxUOQfr = false;}
      if(pQdyOQHQTY == true){pQdyOQHQTY = false;}
      if(sZAUlHqirE == true){sZAUlHqirE = false;}
      if(oLMhbnGoxr == true){oLMhbnGoxr = false;}
      if(xGwpNIJCVk == true){xGwpNIJCVk = false;}
      if(FUCcRFWsLp == true){FUCcRFWsLp = false;}
      if(ceXaCeDwWa == true){ceXaCeDwWa = false;}
      if(JRSbblcJGc == true){JRSbblcJGc = false;}
      if(ynNGzTJjPV == true){ynNGzTJjPV = false;}
      if(QMdHORhVmi == true){QMdHORhVmi = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class QEFMYHGDCA
{ 
  void TAGXFBmcyN()
  { 
      bool OhrHfXYwhV = false;
      bool LMzbqnGpkl = false;
      bool DOxtXozCED = false;
      bool dwoEaqFSiU = false;
      bool SszWmlMNLa = false;
      bool NwZwzzkyiq = false;
      bool XVqeqOXtAz = false;
      bool inHQTKPZHk = false;
      bool clpdphWrEa = false;
      bool GHirwwCTeX = false;
      bool ORNMrZnIbo = false;
      bool jKqxOleNoJ = false;
      bool repYraEPUW = false;
      bool IuuiWKXdkQ = false;
      bool sNQOqwPkcK = false;
      bool QneVeibnTQ = false;
      bool UYnEgoOHaD = false;
      bool NBbYPeOahw = false;
      bool KSlKFTPZEn = false;
      bool ijJTuUZLhB = false;
      string WEfSdFQFPV;
      string FAgAyoTqGz;
      string raCfYSCstW;
      string NzbltzuozG;
      string iQgkCoOTRK;
      string zcDBauCztL;
      string DXKsHFXtFy;
      string tdrAQatQci;
      string DhajWkwZLz;
      string xuumCKFmcx;
      string RTOTPUkDjz;
      string BkWKHUxaVW;
      string JxiGByiExG;
      string VGnfRjTHUB;
      string zrpLyLYkeh;
      string NeQFmTjHRQ;
      string lfFdctxprX;
      string BoFUWQHWIp;
      string ZyZPrqphsy;
      string zLoDPTlLhD;
      if(WEfSdFQFPV == RTOTPUkDjz){OhrHfXYwhV = true;}
      else if(RTOTPUkDjz == WEfSdFQFPV){ORNMrZnIbo = true;}
      if(FAgAyoTqGz == BkWKHUxaVW){LMzbqnGpkl = true;}
      else if(BkWKHUxaVW == FAgAyoTqGz){jKqxOleNoJ = true;}
      if(raCfYSCstW == JxiGByiExG){DOxtXozCED = true;}
      else if(JxiGByiExG == raCfYSCstW){repYraEPUW = true;}
      if(NzbltzuozG == VGnfRjTHUB){dwoEaqFSiU = true;}
      else if(VGnfRjTHUB == NzbltzuozG){IuuiWKXdkQ = true;}
      if(iQgkCoOTRK == zrpLyLYkeh){SszWmlMNLa = true;}
      else if(zrpLyLYkeh == iQgkCoOTRK){sNQOqwPkcK = true;}
      if(zcDBauCztL == NeQFmTjHRQ){NwZwzzkyiq = true;}
      else if(NeQFmTjHRQ == zcDBauCztL){QneVeibnTQ = true;}
      if(DXKsHFXtFy == lfFdctxprX){XVqeqOXtAz = true;}
      else if(lfFdctxprX == DXKsHFXtFy){UYnEgoOHaD = true;}
      if(tdrAQatQci == BoFUWQHWIp){inHQTKPZHk = true;}
      if(DhajWkwZLz == ZyZPrqphsy){clpdphWrEa = true;}
      if(xuumCKFmcx == zLoDPTlLhD){GHirwwCTeX = true;}
      while(BoFUWQHWIp == tdrAQatQci){NBbYPeOahw = true;}
      while(ZyZPrqphsy == ZyZPrqphsy){KSlKFTPZEn = true;}
      while(zLoDPTlLhD == zLoDPTlLhD){ijJTuUZLhB = true;}
      if(OhrHfXYwhV == true){OhrHfXYwhV = false;}
      if(LMzbqnGpkl == true){LMzbqnGpkl = false;}
      if(DOxtXozCED == true){DOxtXozCED = false;}
      if(dwoEaqFSiU == true){dwoEaqFSiU = false;}
      if(SszWmlMNLa == true){SszWmlMNLa = false;}
      if(NwZwzzkyiq == true){NwZwzzkyiq = false;}
      if(XVqeqOXtAz == true){XVqeqOXtAz = false;}
      if(inHQTKPZHk == true){inHQTKPZHk = false;}
      if(clpdphWrEa == true){clpdphWrEa = false;}
      if(GHirwwCTeX == true){GHirwwCTeX = false;}
      if(ORNMrZnIbo == true){ORNMrZnIbo = false;}
      if(jKqxOleNoJ == true){jKqxOleNoJ = false;}
      if(repYraEPUW == true){repYraEPUW = false;}
      if(IuuiWKXdkQ == true){IuuiWKXdkQ = false;}
      if(sNQOqwPkcK == true){sNQOqwPkcK = false;}
      if(QneVeibnTQ == true){QneVeibnTQ = false;}
      if(UYnEgoOHaD == true){UYnEgoOHaD = false;}
      if(NBbYPeOahw == true){NBbYPeOahw = false;}
      if(KSlKFTPZEn == true){KSlKFTPZEn = false;}
      if(ijJTuUZLhB == true){ijJTuUZLhB = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class MMXJLGYDEB
{ 
  void cMDrSsVhSI()
  { 
      bool dTYAbBWfQc = false;
      bool HfhEdFsnrm = false;
      bool HGcMDKOIDA = false;
      bool fhxTqXrOeM = false;
      bool pqhOMfAiqW = false;
      bool CQDXEWqLwR = false;
      bool mObQwYhNNF = false;
      bool NdJewPYOAX = false;
      bool JfQgyOdYOT = false;
      bool ggRNZbPRRm = false;
      bool xlVkJXHXbo = false;
      bool ycKYIGxuSh = false;
      bool uIzsYfLuET = false;
      bool AGwNZWKfWZ = false;
      bool HFwYUEVsFT = false;
      bool eUzjrnmbMc = false;
      bool LMTIorfwyk = false;
      bool wRXrZoLZYT = false;
      bool JSpkofHMVk = false;
      bool jhSsNtlnjc = false;
      string sTBJjaiLgf;
      string uzbfBqNMWJ;
      string bAbTANBctE;
      string ojoeAtuzCj;
      string XoWDMcERly;
      string IxKLDrqgIW;
      string YFRcqLKwfH;
      string uNKmkmdYGW;
      string IiquuqYcbH;
      string COqeKSXsMB;
      string SIldDtmXZP;
      string OSZkOpOVgz;
      string lgUXFgWebS;
      string PjMFEzwMrA;
      string DBDLrjnFPU;
      string DBMMOknNYW;
      string XIuHAeGTgO;
      string sCdgukdIfU;
      string rRPHONWzqT;
      string LimVcuFwKM;
      if(sTBJjaiLgf == SIldDtmXZP){dTYAbBWfQc = true;}
      else if(SIldDtmXZP == sTBJjaiLgf){xlVkJXHXbo = true;}
      if(uzbfBqNMWJ == OSZkOpOVgz){HfhEdFsnrm = true;}
      else if(OSZkOpOVgz == uzbfBqNMWJ){ycKYIGxuSh = true;}
      if(bAbTANBctE == lgUXFgWebS){HGcMDKOIDA = true;}
      else if(lgUXFgWebS == bAbTANBctE){uIzsYfLuET = true;}
      if(ojoeAtuzCj == PjMFEzwMrA){fhxTqXrOeM = true;}
      else if(PjMFEzwMrA == ojoeAtuzCj){AGwNZWKfWZ = true;}
      if(XoWDMcERly == DBDLrjnFPU){pqhOMfAiqW = true;}
      else if(DBDLrjnFPU == XoWDMcERly){HFwYUEVsFT = true;}
      if(IxKLDrqgIW == DBMMOknNYW){CQDXEWqLwR = true;}
      else if(DBMMOknNYW == IxKLDrqgIW){eUzjrnmbMc = true;}
      if(YFRcqLKwfH == XIuHAeGTgO){mObQwYhNNF = true;}
      else if(XIuHAeGTgO == YFRcqLKwfH){LMTIorfwyk = true;}
      if(uNKmkmdYGW == sCdgukdIfU){NdJewPYOAX = true;}
      if(IiquuqYcbH == rRPHONWzqT){JfQgyOdYOT = true;}
      if(COqeKSXsMB == LimVcuFwKM){ggRNZbPRRm = true;}
      while(sCdgukdIfU == uNKmkmdYGW){wRXrZoLZYT = true;}
      while(rRPHONWzqT == rRPHONWzqT){JSpkofHMVk = true;}
      while(LimVcuFwKM == LimVcuFwKM){jhSsNtlnjc = true;}
      if(dTYAbBWfQc == true){dTYAbBWfQc = false;}
      if(HfhEdFsnrm == true){HfhEdFsnrm = false;}
      if(HGcMDKOIDA == true){HGcMDKOIDA = false;}
      if(fhxTqXrOeM == true){fhxTqXrOeM = false;}
      if(pqhOMfAiqW == true){pqhOMfAiqW = false;}
      if(CQDXEWqLwR == true){CQDXEWqLwR = false;}
      if(mObQwYhNNF == true){mObQwYhNNF = false;}
      if(NdJewPYOAX == true){NdJewPYOAX = false;}
      if(JfQgyOdYOT == true){JfQgyOdYOT = false;}
      if(ggRNZbPRRm == true){ggRNZbPRRm = false;}
      if(xlVkJXHXbo == true){xlVkJXHXbo = false;}
      if(ycKYIGxuSh == true){ycKYIGxuSh = false;}
      if(uIzsYfLuET == true){uIzsYfLuET = false;}
      if(AGwNZWKfWZ == true){AGwNZWKfWZ = false;}
      if(HFwYUEVsFT == true){HFwYUEVsFT = false;}
      if(eUzjrnmbMc == true){eUzjrnmbMc = false;}
      if(LMTIorfwyk == true){LMTIorfwyk = false;}
      if(wRXrZoLZYT == true){wRXrZoLZYT = false;}
      if(JSpkofHMVk == true){JSpkofHMVk = false;}
      if(jhSsNtlnjc == true){jhSsNtlnjc = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FDIQWWMOTU
{ 
  void KXCRXeFUBE()
  { 
      bool ILCekGdGXN = false;
      bool OAAxdslTal = false;
      bool AQsNqboaUk = false;
      bool wEZCIYbDTz = false;
      bool WitPNFjpBt = false;
      bool kqVOYrRnnJ = false;
      bool RClmRJaxnF = false;
      bool QhzbOQWdAk = false;
      bool ryZaAFhKdQ = false;
      bool GnPKhUwGyD = false;
      bool elCucCqszH = false;
      bool VnnCRScKmL = false;
      bool aPSWycRott = false;
      bool TPewGFwdxx = false;
      bool NoLWuSzMsP = false;
      bool qlSlbOTexA = false;
      bool jCzEmFIqwL = false;
      bool VyNpylEwbe = false;
      bool cFweDhwsUq = false;
      bool lRgxMLoGjp = false;
      string XAVkjBxOQC;
      string VHfsZToawb;
      string jpmjPjznks;
      string bOaGrostHX;
      string XNDHVbOTLJ;
      string oBBatgNlUA;
      string CIcWaGgZBk;
      string auTYEVlfOm;
      string VecbEZNALK;
      string aGYGUOkggO;
      string unSZhshcVX;
      string uhXgdXfASA;
      string rfNqKTVMDo;
      string kEyzrgLCtK;
      string rkdnOriqcx;
      string ijOfVPabPO;
      string XVZgmpiXox;
      string LcGHHHBZbR;
      string jdiFFApSdb;
      string GjmemWLxAY;
      if(XAVkjBxOQC == unSZhshcVX){ILCekGdGXN = true;}
      else if(unSZhshcVX == XAVkjBxOQC){elCucCqszH = true;}
      if(VHfsZToawb == uhXgdXfASA){OAAxdslTal = true;}
      else if(uhXgdXfASA == VHfsZToawb){VnnCRScKmL = true;}
      if(jpmjPjznks == rfNqKTVMDo){AQsNqboaUk = true;}
      else if(rfNqKTVMDo == jpmjPjznks){aPSWycRott = true;}
      if(bOaGrostHX == kEyzrgLCtK){wEZCIYbDTz = true;}
      else if(kEyzrgLCtK == bOaGrostHX){TPewGFwdxx = true;}
      if(XNDHVbOTLJ == rkdnOriqcx){WitPNFjpBt = true;}
      else if(rkdnOriqcx == XNDHVbOTLJ){NoLWuSzMsP = true;}
      if(oBBatgNlUA == ijOfVPabPO){kqVOYrRnnJ = true;}
      else if(ijOfVPabPO == oBBatgNlUA){qlSlbOTexA = true;}
      if(CIcWaGgZBk == XVZgmpiXox){RClmRJaxnF = true;}
      else if(XVZgmpiXox == CIcWaGgZBk){jCzEmFIqwL = true;}
      if(auTYEVlfOm == LcGHHHBZbR){QhzbOQWdAk = true;}
      if(VecbEZNALK == jdiFFApSdb){ryZaAFhKdQ = true;}
      if(aGYGUOkggO == GjmemWLxAY){GnPKhUwGyD = true;}
      while(LcGHHHBZbR == auTYEVlfOm){VyNpylEwbe = true;}
      while(jdiFFApSdb == jdiFFApSdb){cFweDhwsUq = true;}
      while(GjmemWLxAY == GjmemWLxAY){lRgxMLoGjp = true;}
      if(ILCekGdGXN == true){ILCekGdGXN = false;}
      if(OAAxdslTal == true){OAAxdslTal = false;}
      if(AQsNqboaUk == true){AQsNqboaUk = false;}
      if(wEZCIYbDTz == true){wEZCIYbDTz = false;}
      if(WitPNFjpBt == true){WitPNFjpBt = false;}
      if(kqVOYrRnnJ == true){kqVOYrRnnJ = false;}
      if(RClmRJaxnF == true){RClmRJaxnF = false;}
      if(QhzbOQWdAk == true){QhzbOQWdAk = false;}
      if(ryZaAFhKdQ == true){ryZaAFhKdQ = false;}
      if(GnPKhUwGyD == true){GnPKhUwGyD = false;}
      if(elCucCqszH == true){elCucCqszH = false;}
      if(VnnCRScKmL == true){VnnCRScKmL = false;}
      if(aPSWycRott == true){aPSWycRott = false;}
      if(TPewGFwdxx == true){TPewGFwdxx = false;}
      if(NoLWuSzMsP == true){NoLWuSzMsP = false;}
      if(qlSlbOTexA == true){qlSlbOTexA = false;}
      if(jCzEmFIqwL == true){jCzEmFIqwL = false;}
      if(VyNpylEwbe == true){VyNpylEwbe = false;}
      if(cFweDhwsUq == true){cFweDhwsUq = false;}
      if(lRgxMLoGjp == true){lRgxMLoGjp = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class RERKOBIVPF
{ 
  void HVsAKZfyBw()
  { 
      bool TMuaiBXVQI = false;
      bool xNYoRAxnZG = false;
      bool bESLSwxfwb = false;
      bool ebQsXlpFGC = false;
      bool WodaJmzFQA = false;
      bool kJjsoOsiHl = false;
      bool DqYocVweQf = false;
      bool KrhXjmarCf = false;
      bool xqOwqVqXEI = false;
      bool rHxTyePoyn = false;
      bool qTokosaMJC = false;
      bool nBfbnLccxG = false;
      bool dYZHmCtNZE = false;
      bool WktOocCdGx = false;
      bool teNCTpaVfl = false;
      bool BmqqJRsgxl = false;
      bool yNlEeDHRlL = false;
      bool JzVAMzKXnw = false;
      bool bYhmDisDFW = false;
      bool iVfTzgrIWQ = false;
      string BMqRFfIFyi;
      string yqAJscJusO;
      string cAPGFbtSMl;
      string suAzWoZkYS;
      string DtQqsCylxh;
      string LbxkQBxkMd;
      string kpsLpCqBlE;
      string CYWZKhiYMC;
      string cIVeaRyPiV;
      string rnXugNBEAq;
      string LxzsemEWah;
      string RXYJAcUOkX;
      string EbRPLOAasw;
      string gdXVqfIbzn;
      string ManAETPOuD;
      string smTcljejgQ;
      string ldddAmACST;
      string UrOxLcmRKr;
      string ylfoDtKujG;
      string AXuiBixKAR;
      if(BMqRFfIFyi == LxzsemEWah){TMuaiBXVQI = true;}
      else if(LxzsemEWah == BMqRFfIFyi){qTokosaMJC = true;}
      if(yqAJscJusO == RXYJAcUOkX){xNYoRAxnZG = true;}
      else if(RXYJAcUOkX == yqAJscJusO){nBfbnLccxG = true;}
      if(cAPGFbtSMl == EbRPLOAasw){bESLSwxfwb = true;}
      else if(EbRPLOAasw == cAPGFbtSMl){dYZHmCtNZE = true;}
      if(suAzWoZkYS == gdXVqfIbzn){ebQsXlpFGC = true;}
      else if(gdXVqfIbzn == suAzWoZkYS){WktOocCdGx = true;}
      if(DtQqsCylxh == ManAETPOuD){WodaJmzFQA = true;}
      else if(ManAETPOuD == DtQqsCylxh){teNCTpaVfl = true;}
      if(LbxkQBxkMd == smTcljejgQ){kJjsoOsiHl = true;}
      else if(smTcljejgQ == LbxkQBxkMd){BmqqJRsgxl = true;}
      if(kpsLpCqBlE == ldddAmACST){DqYocVweQf = true;}
      else if(ldddAmACST == kpsLpCqBlE){yNlEeDHRlL = true;}
      if(CYWZKhiYMC == UrOxLcmRKr){KrhXjmarCf = true;}
      if(cIVeaRyPiV == ylfoDtKujG){xqOwqVqXEI = true;}
      if(rnXugNBEAq == AXuiBixKAR){rHxTyePoyn = true;}
      while(UrOxLcmRKr == CYWZKhiYMC){JzVAMzKXnw = true;}
      while(ylfoDtKujG == ylfoDtKujG){bYhmDisDFW = true;}
      while(AXuiBixKAR == AXuiBixKAR){iVfTzgrIWQ = true;}
      if(TMuaiBXVQI == true){TMuaiBXVQI = false;}
      if(xNYoRAxnZG == true){xNYoRAxnZG = false;}
      if(bESLSwxfwb == true){bESLSwxfwb = false;}
      if(ebQsXlpFGC == true){ebQsXlpFGC = false;}
      if(WodaJmzFQA == true){WodaJmzFQA = false;}
      if(kJjsoOsiHl == true){kJjsoOsiHl = false;}
      if(DqYocVweQf == true){DqYocVweQf = false;}
      if(KrhXjmarCf == true){KrhXjmarCf = false;}
      if(xqOwqVqXEI == true){xqOwqVqXEI = false;}
      if(rHxTyePoyn == true){rHxTyePoyn = false;}
      if(qTokosaMJC == true){qTokosaMJC = false;}
      if(nBfbnLccxG == true){nBfbnLccxG = false;}
      if(dYZHmCtNZE == true){dYZHmCtNZE = false;}
      if(WktOocCdGx == true){WktOocCdGx = false;}
      if(teNCTpaVfl == true){teNCTpaVfl = false;}
      if(BmqqJRsgxl == true){BmqqJRsgxl = false;}
      if(yNlEeDHRlL == true){yNlEeDHRlL = false;}
      if(JzVAMzKXnw == true){JzVAMzKXnw = false;}
      if(bYhmDisDFW == true){bYhmDisDFW = false;}
      if(iVfTzgrIWQ == true){iVfTzgrIWQ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class CHWMRKDYOZ
{ 
  void jBmTphIiQN()
  { 
      bool fSBGpqPVrg = false;
      bool TYzaKSTVEe = false;
      bool KNVtUymHDS = false;
      bool URAxkqfhgW = false;
      bool lYVFgQHgVc = false;
      bool jKbatSwHlq = false;
      bool EHSZMCYBoT = false;
      bool lkogrkqPMD = false;
      bool qFkahJoYCR = false;
      bool tXwoReGwse = false;
      bool rFqCpyUtiI = false;
      bool afuhAyWrqC = false;
      bool NzyXYOoAPN = false;
      bool euWhLkNiji = false;
      bool otptqOcOnT = false;
      bool IOEpRsGTdb = false;
      bool IUeVdfINVP = false;
      bool jmHHNSIEgJ = false;
      bool laMuEPqfPS = false;
      bool dtEHZTPUPK = false;
      string aWRGGdfAYI;
      string iejnhOCXYa;
      string qVnEJioTEA;
      string uWQQowVJUx;
      string spnLWbqPDy;
      string YSlAoFhNxX;
      string nLuXMLNTyl;
      string srDbhBhcgh;
      string rZQGbcjQaj;
      string NHMxJgtQzK;
      string OWQQFfPKSn;
      string XFfOeNuouj;
      string awrOYkfGRA;
      string GQdWTUIzAw;
      string MZymLHbxRu;
      string lEUPiReCrT;
      string AIsRgGbJuK;
      string FunzWBxFCQ;
      string JhoWNCuOmB;
      string pGHguEgfFF;
      if(aWRGGdfAYI == OWQQFfPKSn){fSBGpqPVrg = true;}
      else if(OWQQFfPKSn == aWRGGdfAYI){rFqCpyUtiI = true;}
      if(iejnhOCXYa == XFfOeNuouj){TYzaKSTVEe = true;}
      else if(XFfOeNuouj == iejnhOCXYa){afuhAyWrqC = true;}
      if(qVnEJioTEA == awrOYkfGRA){KNVtUymHDS = true;}
      else if(awrOYkfGRA == qVnEJioTEA){NzyXYOoAPN = true;}
      if(uWQQowVJUx == GQdWTUIzAw){URAxkqfhgW = true;}
      else if(GQdWTUIzAw == uWQQowVJUx){euWhLkNiji = true;}
      if(spnLWbqPDy == MZymLHbxRu){lYVFgQHgVc = true;}
      else if(MZymLHbxRu == spnLWbqPDy){otptqOcOnT = true;}
      if(YSlAoFhNxX == lEUPiReCrT){jKbatSwHlq = true;}
      else if(lEUPiReCrT == YSlAoFhNxX){IOEpRsGTdb = true;}
      if(nLuXMLNTyl == AIsRgGbJuK){EHSZMCYBoT = true;}
      else if(AIsRgGbJuK == nLuXMLNTyl){IUeVdfINVP = true;}
      if(srDbhBhcgh == FunzWBxFCQ){lkogrkqPMD = true;}
      if(rZQGbcjQaj == JhoWNCuOmB){qFkahJoYCR = true;}
      if(NHMxJgtQzK == pGHguEgfFF){tXwoReGwse = true;}
      while(FunzWBxFCQ == srDbhBhcgh){jmHHNSIEgJ = true;}
      while(JhoWNCuOmB == JhoWNCuOmB){laMuEPqfPS = true;}
      while(pGHguEgfFF == pGHguEgfFF){dtEHZTPUPK = true;}
      if(fSBGpqPVrg == true){fSBGpqPVrg = false;}
      if(TYzaKSTVEe == true){TYzaKSTVEe = false;}
      if(KNVtUymHDS == true){KNVtUymHDS = false;}
      if(URAxkqfhgW == true){URAxkqfhgW = false;}
      if(lYVFgQHgVc == true){lYVFgQHgVc = false;}
      if(jKbatSwHlq == true){jKbatSwHlq = false;}
      if(EHSZMCYBoT == true){EHSZMCYBoT = false;}
      if(lkogrkqPMD == true){lkogrkqPMD = false;}
      if(qFkahJoYCR == true){qFkahJoYCR = false;}
      if(tXwoReGwse == true){tXwoReGwse = false;}
      if(rFqCpyUtiI == true){rFqCpyUtiI = false;}
      if(afuhAyWrqC == true){afuhAyWrqC = false;}
      if(NzyXYOoAPN == true){NzyXYOoAPN = false;}
      if(euWhLkNiji == true){euWhLkNiji = false;}
      if(otptqOcOnT == true){otptqOcOnT = false;}
      if(IOEpRsGTdb == true){IOEpRsGTdb = false;}
      if(IUeVdfINVP == true){IUeVdfINVP = false;}
      if(jmHHNSIEgJ == true){jmHHNSIEgJ = false;}
      if(laMuEPqfPS == true){laMuEPqfPS = false;}
      if(dtEHZTPUPK == true){dtEHZTPUPK = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JVHQUZOZGJ
{ 
  void irwmkmTOFO()
  { 
      bool ffCjWpxLwf = false;
      bool fGDUigmoVH = false;
      bool HSiPapprQp = false;
      bool dzGPjTaVGw = false;
      bool IzMpmOxpWy = false;
      bool wkzZyMxlJg = false;
      bool AFtFBgznjP = false;
      bool GQiZyHGscM = false;
      bool rAVStsAgRX = false;
      bool VnMZwDFHnG = false;
      bool OJFGrfBZap = false;
      bool GEJxnAiaKI = false;
      bool gSVMsSEJcu = false;
      bool CyFWxitOaN = false;
      bool qZFaQRePku = false;
      bool UdweZwmrNY = false;
      bool brwfmHzmzE = false;
      bool FUAxRGNtwa = false;
      bool mnXhZYCZwB = false;
      bool uBABlzcYnI = false;
      string XIkiriFLIn;
      string pELPNkhnTP;
      string QVufCfTBNB;
      string ZmgMLaCYGu;
      string sfEonbgXuD;
      string jOhKACMEfM;
      string ODxGODXyDl;
      string iIhXVjQFrZ;
      string zwFKRKEVMJ;
      string Oxrtgphxak;
      string EhTJUagKoP;
      string kGMFNSxmnE;
      string zBltXkPmqf;
      string BTeHVQbwoq;
      string dtfwXHklgN;
      string ruIWkCwZki;
      string KsSJORbEjw;
      string qByxaFVoVQ;
      string AcGcmGKIVT;
      string UBKFfjaVhF;
      if(XIkiriFLIn == EhTJUagKoP){ffCjWpxLwf = true;}
      else if(EhTJUagKoP == XIkiriFLIn){OJFGrfBZap = true;}
      if(pELPNkhnTP == kGMFNSxmnE){fGDUigmoVH = true;}
      else if(kGMFNSxmnE == pELPNkhnTP){GEJxnAiaKI = true;}
      if(QVufCfTBNB == zBltXkPmqf){HSiPapprQp = true;}
      else if(zBltXkPmqf == QVufCfTBNB){gSVMsSEJcu = true;}
      if(ZmgMLaCYGu == BTeHVQbwoq){dzGPjTaVGw = true;}
      else if(BTeHVQbwoq == ZmgMLaCYGu){CyFWxitOaN = true;}
      if(sfEonbgXuD == dtfwXHklgN){IzMpmOxpWy = true;}
      else if(dtfwXHklgN == sfEonbgXuD){qZFaQRePku = true;}
      if(jOhKACMEfM == ruIWkCwZki){wkzZyMxlJg = true;}
      else if(ruIWkCwZki == jOhKACMEfM){UdweZwmrNY = true;}
      if(ODxGODXyDl == KsSJORbEjw){AFtFBgznjP = true;}
      else if(KsSJORbEjw == ODxGODXyDl){brwfmHzmzE = true;}
      if(iIhXVjQFrZ == qByxaFVoVQ){GQiZyHGscM = true;}
      if(zwFKRKEVMJ == AcGcmGKIVT){rAVStsAgRX = true;}
      if(Oxrtgphxak == UBKFfjaVhF){VnMZwDFHnG = true;}
      while(qByxaFVoVQ == iIhXVjQFrZ){FUAxRGNtwa = true;}
      while(AcGcmGKIVT == AcGcmGKIVT){mnXhZYCZwB = true;}
      while(UBKFfjaVhF == UBKFfjaVhF){uBABlzcYnI = true;}
      if(ffCjWpxLwf == true){ffCjWpxLwf = false;}
      if(fGDUigmoVH == true){fGDUigmoVH = false;}
      if(HSiPapprQp == true){HSiPapprQp = false;}
      if(dzGPjTaVGw == true){dzGPjTaVGw = false;}
      if(IzMpmOxpWy == true){IzMpmOxpWy = false;}
      if(wkzZyMxlJg == true){wkzZyMxlJg = false;}
      if(AFtFBgznjP == true){AFtFBgznjP = false;}
      if(GQiZyHGscM == true){GQiZyHGscM = false;}
      if(rAVStsAgRX == true){rAVStsAgRX = false;}
      if(VnMZwDFHnG == true){VnMZwDFHnG = false;}
      if(OJFGrfBZap == true){OJFGrfBZap = false;}
      if(GEJxnAiaKI == true){GEJxnAiaKI = false;}
      if(gSVMsSEJcu == true){gSVMsSEJcu = false;}
      if(CyFWxitOaN == true){CyFWxitOaN = false;}
      if(qZFaQRePku == true){qZFaQRePku = false;}
      if(UdweZwmrNY == true){UdweZwmrNY = false;}
      if(brwfmHzmzE == true){brwfmHzmzE = false;}
      if(FUAxRGNtwa == true){FUAxRGNtwa = false;}
      if(mnXhZYCZwB == true){mnXhZYCZwB = false;}
      if(uBABlzcYnI == true){uBABlzcYnI = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FDMOXOGLUZ
{ 
  void jbutSSrSrc()
  { 
      bool FPtYNPnGXf = false;
      bool cKHZAdKCdH = false;
      bool QeWceByYeM = false;
      bool bwaotJpAqD = false;
      bool orykNFMWUG = false;
      bool xGWcGarQsM = false;
      bool QPbLDgjRep = false;
      bool mJKZRYglPZ = false;
      bool nbEXOUZyiT = false;
      bool RZGemZPmgg = false;
      bool UtqeTmwwKh = false;
      bool sFATXnKEwi = false;
      bool GliKXMDemq = false;
      bool IsEiEtkZEb = false;
      bool YdmAZLYmGy = false;
      bool OLqyaiKfGG = false;
      bool iQITZzBROd = false;
      bool DkzUJbyikL = false;
      bool mQYobwCFIV = false;
      bool iFSyrHVSaa = false;
      string FdWQOjoxDe;
      string PpIoeNeZFK;
      string APysnqKRIb;
      string yxPHOBGdaU;
      string BNKinxDxMK;
      string OchmloedcW;
      string YXnafSZBZd;
      string GAykJmEPFp;
      string nNFyMVOcUY;
      string VcBDudYjCV;
      string XBWhDNdxdz;
      string kOYmhLRoNK;
      string DwfBHdztoe;
      string NUNiRlpWuz;
      string qRzVnHEOVi;
      string LiVqRaVVZZ;
      string OiIylPBoUw;
      string VutOTlxKNA;
      string YkYFTdzVRq;
      string QUwUjHJAjX;
      if(FdWQOjoxDe == XBWhDNdxdz){FPtYNPnGXf = true;}
      else if(XBWhDNdxdz == FdWQOjoxDe){UtqeTmwwKh = true;}
      if(PpIoeNeZFK == kOYmhLRoNK){cKHZAdKCdH = true;}
      else if(kOYmhLRoNK == PpIoeNeZFK){sFATXnKEwi = true;}
      if(APysnqKRIb == DwfBHdztoe){QeWceByYeM = true;}
      else if(DwfBHdztoe == APysnqKRIb){GliKXMDemq = true;}
      if(yxPHOBGdaU == NUNiRlpWuz){bwaotJpAqD = true;}
      else if(NUNiRlpWuz == yxPHOBGdaU){IsEiEtkZEb = true;}
      if(BNKinxDxMK == qRzVnHEOVi){orykNFMWUG = true;}
      else if(qRzVnHEOVi == BNKinxDxMK){YdmAZLYmGy = true;}
      if(OchmloedcW == LiVqRaVVZZ){xGWcGarQsM = true;}
      else if(LiVqRaVVZZ == OchmloedcW){OLqyaiKfGG = true;}
      if(YXnafSZBZd == OiIylPBoUw){QPbLDgjRep = true;}
      else if(OiIylPBoUw == YXnafSZBZd){iQITZzBROd = true;}
      if(GAykJmEPFp == VutOTlxKNA){mJKZRYglPZ = true;}
      if(nNFyMVOcUY == YkYFTdzVRq){nbEXOUZyiT = true;}
      if(VcBDudYjCV == QUwUjHJAjX){RZGemZPmgg = true;}
      while(VutOTlxKNA == GAykJmEPFp){DkzUJbyikL = true;}
      while(YkYFTdzVRq == YkYFTdzVRq){mQYobwCFIV = true;}
      while(QUwUjHJAjX == QUwUjHJAjX){iFSyrHVSaa = true;}
      if(FPtYNPnGXf == true){FPtYNPnGXf = false;}
      if(cKHZAdKCdH == true){cKHZAdKCdH = false;}
      if(QeWceByYeM == true){QeWceByYeM = false;}
      if(bwaotJpAqD == true){bwaotJpAqD = false;}
      if(orykNFMWUG == true){orykNFMWUG = false;}
      if(xGWcGarQsM == true){xGWcGarQsM = false;}
      if(QPbLDgjRep == true){QPbLDgjRep = false;}
      if(mJKZRYglPZ == true){mJKZRYglPZ = false;}
      if(nbEXOUZyiT == true){nbEXOUZyiT = false;}
      if(RZGemZPmgg == true){RZGemZPmgg = false;}
      if(UtqeTmwwKh == true){UtqeTmwwKh = false;}
      if(sFATXnKEwi == true){sFATXnKEwi = false;}
      if(GliKXMDemq == true){GliKXMDemq = false;}
      if(IsEiEtkZEb == true){IsEiEtkZEb = false;}
      if(YdmAZLYmGy == true){YdmAZLYmGy = false;}
      if(OLqyaiKfGG == true){OLqyaiKfGG = false;}
      if(iQITZzBROd == true){iQITZzBROd = false;}
      if(DkzUJbyikL == true){DkzUJbyikL = false;}
      if(mQYobwCFIV == true){mQYobwCFIV = false;}
      if(iFSyrHVSaa == true){iFSyrHVSaa = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class EIYVCNSYNN
{ 
  void jQHMjWuVyM()
  { 
      bool CeKhZKhkDk = false;
      bool iVGVTecOuy = false;
      bool NeWCBuPnpw = false;
      bool AWTlRPAFGJ = false;
      bool dzGItfHeyu = false;
      bool aiwFnWHHfe = false;
      bool tstGkMBUDE = false;
      bool eXIlUNYNHV = false;
      bool mkpuVQaOxu = false;
      bool HrZaOjEzXh = false;
      bool emasQDaKkZ = false;
      bool xtAJrJDzkT = false;
      bool klWIxlygWj = false;
      bool JYoYqAJweS = false;
      bool pQdXxNrPiF = false;
      bool gFOEwrPSwd = false;
      bool FsmAQXEaSq = false;
      bool zGTScUHzKi = false;
      bool thyQCDKkcy = false;
      bool jqAAMxccHZ = false;
      string NoUuoDssuo;
      string egWVrQDQdE;
      string eCwXsQtwTD;
      string IiyMxmlBHK;
      string cBcHUzpFFi;
      string txejnLrwCY;
      string IwYqguhKYF;
      string eBISacYoCs;
      string ZopkJKEOYl;
      string toziCgckaZ;
      string RegADroLIK;
      string lDsIySGnhV;
      string qTlmKoBYQt;
      string AnNcbtIpgQ;
      string UsFNzquqkj;
      string anCEixstoO;
      string pQuOJyoQgD;
      string JQGRBxNXmT;
      string tKmfTLkJtd;
      string BOASaExqDB;
      if(NoUuoDssuo == RegADroLIK){CeKhZKhkDk = true;}
      else if(RegADroLIK == NoUuoDssuo){emasQDaKkZ = true;}
      if(egWVrQDQdE == lDsIySGnhV){iVGVTecOuy = true;}
      else if(lDsIySGnhV == egWVrQDQdE){xtAJrJDzkT = true;}
      if(eCwXsQtwTD == qTlmKoBYQt){NeWCBuPnpw = true;}
      else if(qTlmKoBYQt == eCwXsQtwTD){klWIxlygWj = true;}
      if(IiyMxmlBHK == AnNcbtIpgQ){AWTlRPAFGJ = true;}
      else if(AnNcbtIpgQ == IiyMxmlBHK){JYoYqAJweS = true;}
      if(cBcHUzpFFi == UsFNzquqkj){dzGItfHeyu = true;}
      else if(UsFNzquqkj == cBcHUzpFFi){pQdXxNrPiF = true;}
      if(txejnLrwCY == anCEixstoO){aiwFnWHHfe = true;}
      else if(anCEixstoO == txejnLrwCY){gFOEwrPSwd = true;}
      if(IwYqguhKYF == pQuOJyoQgD){tstGkMBUDE = true;}
      else if(pQuOJyoQgD == IwYqguhKYF){FsmAQXEaSq = true;}
      if(eBISacYoCs == JQGRBxNXmT){eXIlUNYNHV = true;}
      if(ZopkJKEOYl == tKmfTLkJtd){mkpuVQaOxu = true;}
      if(toziCgckaZ == BOASaExqDB){HrZaOjEzXh = true;}
      while(JQGRBxNXmT == eBISacYoCs){zGTScUHzKi = true;}
      while(tKmfTLkJtd == tKmfTLkJtd){thyQCDKkcy = true;}
      while(BOASaExqDB == BOASaExqDB){jqAAMxccHZ = true;}
      if(CeKhZKhkDk == true){CeKhZKhkDk = false;}
      if(iVGVTecOuy == true){iVGVTecOuy = false;}
      if(NeWCBuPnpw == true){NeWCBuPnpw = false;}
      if(AWTlRPAFGJ == true){AWTlRPAFGJ = false;}
      if(dzGItfHeyu == true){dzGItfHeyu = false;}
      if(aiwFnWHHfe == true){aiwFnWHHfe = false;}
      if(tstGkMBUDE == true){tstGkMBUDE = false;}
      if(eXIlUNYNHV == true){eXIlUNYNHV = false;}
      if(mkpuVQaOxu == true){mkpuVQaOxu = false;}
      if(HrZaOjEzXh == true){HrZaOjEzXh = false;}
      if(emasQDaKkZ == true){emasQDaKkZ = false;}
      if(xtAJrJDzkT == true){xtAJrJDzkT = false;}
      if(klWIxlygWj == true){klWIxlygWj = false;}
      if(JYoYqAJweS == true){JYoYqAJweS = false;}
      if(pQdXxNrPiF == true){pQdXxNrPiF = false;}
      if(gFOEwrPSwd == true){gFOEwrPSwd = false;}
      if(FsmAQXEaSq == true){FsmAQXEaSq = false;}
      if(zGTScUHzKi == true){zGTScUHzKi = false;}
      if(thyQCDKkcy == true){thyQCDKkcy = false;}
      if(jqAAMxccHZ == true){jqAAMxccHZ = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class IBGBIMWTAU
{ 
  void EXiyQfgTfp()
  { 
      bool lwfDXimUlC = false;
      bool PHcduKhOFj = false;
      bool uFtIDCwSll = false;
      bool RqhWsxIUPb = false;
      bool WtdLiKEFXQ = false;
      bool RNgkTfCXAU = false;
      bool lUhzkXYGjx = false;
      bool CMycMAahYr = false;
      bool ypuVUDJwCM = false;
      bool ujUuoCMeaK = false;
      bool kxVZOEODXT = false;
      bool ETyHVwYMdg = false;
      bool OcEWBRrDnK = false;
      bool nwHOHAUUGQ = false;
      bool oGYiIblMYr = false;
      bool dzAswIsyXw = false;
      bool LLPNeILjAV = false;
      bool bDFMrIEUOZ = false;
      bool ffeugowNRz = false;
      bool HVwKEdqLDz = false;
      string fmCYNICGto;
      string QZyUcBQyiX;
      string MuZCLGBlOy;
      string kOBfVbZAdx;
      string GJVVWwybwQ;
      string sYjHijMzNG;
      string HQquECYRzA;
      string aPUlQCZEfO;
      string IZoxELdsxk;
      string OZmWopSZQU;
      string XfcupcEypf;
      string KITyygasps;
      string YeUNlAcxwa;
      string jzmKqcxtHb;
      string GeCRaGGnYI;
      string PYjXwrKPJo;
      string zGCPuLUJHU;
      string jgkOlscDle;
      string toYnKKPPOy;
      string RgjSbzblVP;
      if(fmCYNICGto == XfcupcEypf){lwfDXimUlC = true;}
      else if(XfcupcEypf == fmCYNICGto){kxVZOEODXT = true;}
      if(QZyUcBQyiX == KITyygasps){PHcduKhOFj = true;}
      else if(KITyygasps == QZyUcBQyiX){ETyHVwYMdg = true;}
      if(MuZCLGBlOy == YeUNlAcxwa){uFtIDCwSll = true;}
      else if(YeUNlAcxwa == MuZCLGBlOy){OcEWBRrDnK = true;}
      if(kOBfVbZAdx == jzmKqcxtHb){RqhWsxIUPb = true;}
      else if(jzmKqcxtHb == kOBfVbZAdx){nwHOHAUUGQ = true;}
      if(GJVVWwybwQ == GeCRaGGnYI){WtdLiKEFXQ = true;}
      else if(GeCRaGGnYI == GJVVWwybwQ){oGYiIblMYr = true;}
      if(sYjHijMzNG == PYjXwrKPJo){RNgkTfCXAU = true;}
      else if(PYjXwrKPJo == sYjHijMzNG){dzAswIsyXw = true;}
      if(HQquECYRzA == zGCPuLUJHU){lUhzkXYGjx = true;}
      else if(zGCPuLUJHU == HQquECYRzA){LLPNeILjAV = true;}
      if(aPUlQCZEfO == jgkOlscDle){CMycMAahYr = true;}
      if(IZoxELdsxk == toYnKKPPOy){ypuVUDJwCM = true;}
      if(OZmWopSZQU == RgjSbzblVP){ujUuoCMeaK = true;}
      while(jgkOlscDle == aPUlQCZEfO){bDFMrIEUOZ = true;}
      while(toYnKKPPOy == toYnKKPPOy){ffeugowNRz = true;}
      while(RgjSbzblVP == RgjSbzblVP){HVwKEdqLDz = true;}
      if(lwfDXimUlC == true){lwfDXimUlC = false;}
      if(PHcduKhOFj == true){PHcduKhOFj = false;}
      if(uFtIDCwSll == true){uFtIDCwSll = false;}
      if(RqhWsxIUPb == true){RqhWsxIUPb = false;}
      if(WtdLiKEFXQ == true){WtdLiKEFXQ = false;}
      if(RNgkTfCXAU == true){RNgkTfCXAU = false;}
      if(lUhzkXYGjx == true){lUhzkXYGjx = false;}
      if(CMycMAahYr == true){CMycMAahYr = false;}
      if(ypuVUDJwCM == true){ypuVUDJwCM = false;}
      if(ujUuoCMeaK == true){ujUuoCMeaK = false;}
      if(kxVZOEODXT == true){kxVZOEODXT = false;}
      if(ETyHVwYMdg == true){ETyHVwYMdg = false;}
      if(OcEWBRrDnK == true){OcEWBRrDnK = false;}
      if(nwHOHAUUGQ == true){nwHOHAUUGQ = false;}
      if(oGYiIblMYr == true){oGYiIblMYr = false;}
      if(dzAswIsyXw == true){dzAswIsyXw = false;}
      if(LLPNeILjAV == true){LLPNeILjAV = false;}
      if(bDFMrIEUOZ == true){bDFMrIEUOZ = false;}
      if(ffeugowNRz == true){ffeugowNRz = false;}
      if(HVwKEdqLDz == true){HVwKEdqLDz = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class FHJCPIMCMN
{ 
  void pBlflojXBm()
  { 
      bool gZtiGzQozo = false;
      bool OjXnTPZEPg = false;
      bool PGBKFtrCcF = false;
      bool HKXRrGdOfL = false;
      bool UcVPLwUJIx = false;
      bool okomjZYidL = false;
      bool GNMtgDLGuN = false;
      bool lLYDMmkkyI = false;
      bool YitfPWVjhS = false;
      bool htjTFGQahH = false;
      bool kONuKcLlaw = false;
      bool omkUmWTfeK = false;
      bool hbYceqbEyF = false;
      bool WJCpXKUyUl = false;
      bool KiIkctJPgh = false;
      bool ljwXZIokPT = false;
      bool BHnUZfjtxb = false;
      bool CVKrzzSnzj = false;
      bool gLWPSlMeQO = false;
      bool UihBkDunLx = false;
      string jUVqOWVBur;
      string JizhWdWOJo;
      string ufiNRAbNzK;
      string FYccSMlweL;
      string yHJQCigDGG;
      string QaUTnDkIZO;
      string uHCkmjGWOK;
      string qZixqmWSKi;
      string SXHsseEIoO;
      string adWQQpFXcS;
      string cJowOqqWbA;
      string gRxquWAJrI;
      string exgziNDSfB;
      string gypqGJiKdB;
      string mggnhIfeUt;
      string oufUmecoNB;
      string nXqaShbFkl;
      string npepUyQsQZ;
      string gXxlLkSVEa;
      string fsxcNPpFUz;
      if(jUVqOWVBur == cJowOqqWbA){gZtiGzQozo = true;}
      else if(cJowOqqWbA == jUVqOWVBur){kONuKcLlaw = true;}
      if(JizhWdWOJo == gRxquWAJrI){OjXnTPZEPg = true;}
      else if(gRxquWAJrI == JizhWdWOJo){omkUmWTfeK = true;}
      if(ufiNRAbNzK == exgziNDSfB){PGBKFtrCcF = true;}
      else if(exgziNDSfB == ufiNRAbNzK){hbYceqbEyF = true;}
      if(FYccSMlweL == gypqGJiKdB){HKXRrGdOfL = true;}
      else if(gypqGJiKdB == FYccSMlweL){WJCpXKUyUl = true;}
      if(yHJQCigDGG == mggnhIfeUt){UcVPLwUJIx = true;}
      else if(mggnhIfeUt == yHJQCigDGG){KiIkctJPgh = true;}
      if(QaUTnDkIZO == oufUmecoNB){okomjZYidL = true;}
      else if(oufUmecoNB == QaUTnDkIZO){ljwXZIokPT = true;}
      if(uHCkmjGWOK == nXqaShbFkl){GNMtgDLGuN = true;}
      else if(nXqaShbFkl == uHCkmjGWOK){BHnUZfjtxb = true;}
      if(qZixqmWSKi == npepUyQsQZ){lLYDMmkkyI = true;}
      if(SXHsseEIoO == gXxlLkSVEa){YitfPWVjhS = true;}
      if(adWQQpFXcS == fsxcNPpFUz){htjTFGQahH = true;}
      while(npepUyQsQZ == qZixqmWSKi){CVKrzzSnzj = true;}
      while(gXxlLkSVEa == gXxlLkSVEa){gLWPSlMeQO = true;}
      while(fsxcNPpFUz == fsxcNPpFUz){UihBkDunLx = true;}
      if(gZtiGzQozo == true){gZtiGzQozo = false;}
      if(OjXnTPZEPg == true){OjXnTPZEPg = false;}
      if(PGBKFtrCcF == true){PGBKFtrCcF = false;}
      if(HKXRrGdOfL == true){HKXRrGdOfL = false;}
      if(UcVPLwUJIx == true){UcVPLwUJIx = false;}
      if(okomjZYidL == true){okomjZYidL = false;}
      if(GNMtgDLGuN == true){GNMtgDLGuN = false;}
      if(lLYDMmkkyI == true){lLYDMmkkyI = false;}
      if(YitfPWVjhS == true){YitfPWVjhS = false;}
      if(htjTFGQahH == true){htjTFGQahH = false;}
      if(kONuKcLlaw == true){kONuKcLlaw = false;}
      if(omkUmWTfeK == true){omkUmWTfeK = false;}
      if(hbYceqbEyF == true){hbYceqbEyF = false;}
      if(WJCpXKUyUl == true){WJCpXKUyUl = false;}
      if(KiIkctJPgh == true){KiIkctJPgh = false;}
      if(ljwXZIokPT == true){ljwXZIokPT = false;}
      if(BHnUZfjtxb == true){BHnUZfjtxb = false;}
      if(CVKrzzSnzj == true){CVKrzzSnzj = false;}
      if(gLWPSlMeQO == true){gLWPSlMeQO = false;}
      if(UihBkDunLx == true){UihBkDunLx = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class HCZPRWPIRO
{ 
  void xTPPqWgbgY()
  { 
      bool HzStRXbyWq = false;
      bool ghaNVaFDDY = false;
      bool zgsFHrMcpV = false;
      bool ilREXXjXng = false;
      bool VHxhaZikDc = false;
      bool SxKuBaFMmZ = false;
      bool LyIBIuPVpy = false;
      bool phOVCriLdh = false;
      bool JbYMCkaVCx = false;
      bool RXkOVXyuZf = false;
      bool JMuFmkiosJ = false;
      bool qthFPiZwyW = false;
      bool TtSrfSwAze = false;
      bool smkyXwgTYR = false;
      bool ClcsFWoeyl = false;
      bool xCxCLOtCYU = false;
      bool nioNPEIjgT = false;
      bool CFWgThRZmI = false;
      bool cRBMgqphmI = false;
      bool CduEdTxOcB = false;
      string mLrlEQZnJD;
      string UsmBOREncx;
      string yPfuopqtQi;
      string UZASgclhNm;
      string SWunTwRKTO;
      string kZbdaXexwN;
      string YeXZitsUUl;
      string eVaodbTqas;
      string XfaCxtyVqJ;
      string xokzNnzVFY;
      string DxphiOzlzz;
      string DnVDprEUcn;
      string wWcKopkbbp;
      string bylCoaprSh;
      string uLWqFJSgJU;
      string jdrQbgTfss;
      string dXKJWEBTOO;
      string CLCtskzlFe;
      string WDRGFyFUZI;
      string HJzZEXBNUA;
      if(mLrlEQZnJD == DxphiOzlzz){HzStRXbyWq = true;}
      else if(DxphiOzlzz == mLrlEQZnJD){JMuFmkiosJ = true;}
      if(UsmBOREncx == DnVDprEUcn){ghaNVaFDDY = true;}
      else if(DnVDprEUcn == UsmBOREncx){qthFPiZwyW = true;}
      if(yPfuopqtQi == wWcKopkbbp){zgsFHrMcpV = true;}
      else if(wWcKopkbbp == yPfuopqtQi){TtSrfSwAze = true;}
      if(UZASgclhNm == bylCoaprSh){ilREXXjXng = true;}
      else if(bylCoaprSh == UZASgclhNm){smkyXwgTYR = true;}
      if(SWunTwRKTO == uLWqFJSgJU){VHxhaZikDc = true;}
      else if(uLWqFJSgJU == SWunTwRKTO){ClcsFWoeyl = true;}
      if(kZbdaXexwN == jdrQbgTfss){SxKuBaFMmZ = true;}
      else if(jdrQbgTfss == kZbdaXexwN){xCxCLOtCYU = true;}
      if(YeXZitsUUl == dXKJWEBTOO){LyIBIuPVpy = true;}
      else if(dXKJWEBTOO == YeXZitsUUl){nioNPEIjgT = true;}
      if(eVaodbTqas == CLCtskzlFe){phOVCriLdh = true;}
      if(XfaCxtyVqJ == WDRGFyFUZI){JbYMCkaVCx = true;}
      if(xokzNnzVFY == HJzZEXBNUA){RXkOVXyuZf = true;}
      while(CLCtskzlFe == eVaodbTqas){CFWgThRZmI = true;}
      while(WDRGFyFUZI == WDRGFyFUZI){cRBMgqphmI = true;}
      while(HJzZEXBNUA == HJzZEXBNUA){CduEdTxOcB = true;}
      if(HzStRXbyWq == true){HzStRXbyWq = false;}
      if(ghaNVaFDDY == true){ghaNVaFDDY = false;}
      if(zgsFHrMcpV == true){zgsFHrMcpV = false;}
      if(ilREXXjXng == true){ilREXXjXng = false;}
      if(VHxhaZikDc == true){VHxhaZikDc = false;}
      if(SxKuBaFMmZ == true){SxKuBaFMmZ = false;}
      if(LyIBIuPVpy == true){LyIBIuPVpy = false;}
      if(phOVCriLdh == true){phOVCriLdh = false;}
      if(JbYMCkaVCx == true){JbYMCkaVCx = false;}
      if(RXkOVXyuZf == true){RXkOVXyuZf = false;}
      if(JMuFmkiosJ == true){JMuFmkiosJ = false;}
      if(qthFPiZwyW == true){qthFPiZwyW = false;}
      if(TtSrfSwAze == true){TtSrfSwAze = false;}
      if(smkyXwgTYR == true){smkyXwgTYR = false;}
      if(ClcsFWoeyl == true){ClcsFWoeyl = false;}
      if(xCxCLOtCYU == true){xCxCLOtCYU = false;}
      if(nioNPEIjgT == true){nioNPEIjgT = false;}
      if(CFWgThRZmI == true){CFWgThRZmI = false;}
      if(cRBMgqphmI == true){cRBMgqphmI = false;}
      if(CduEdTxOcB == true){CduEdTxOcB = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class HGENWZEOUE
{ 
  void ZDVKwYsYGQ()
  { 
      bool qNKWYadebY = false;
      bool yHCmGncnnM = false;
      bool iqGBePCtHi = false;
      bool EhIcCapTZG = false;
      bool kpthfGLVYi = false;
      bool lpOwpAGIqI = false;
      bool YjWrTPzYje = false;
      bool yJNgbswXVY = false;
      bool EfUYwOTMmC = false;
      bool UgbQhBSBNP = false;
      bool HlkskcbgUw = false;
      bool DhmSErOywF = false;
      bool wmlgKQgxzZ = false;
      bool ZtKGRSubmT = false;
      bool mlrpzlsJMp = false;
      bool lUFISUzEiT = false;
      bool wdhtWIXgAl = false;
      bool xlKcSJtwhX = false;
      bool XWkImFoBmx = false;
      bool SpsSWIExTq = false;
      string nSTPiizAeo;
      string qujarQAHyg;
      string IVEYUhiWPu;
      string uoVyfcxcAy;
      string UmmBGQPfxd;
      string DjlFQSNtuA;
      string AckjsBZktO;
      string SXHzMUhDSD;
      string SDYkrLDQgN;
      string OCgzyqCCHE;
      string qdkhKebmru;
      string jJbqhwDzrh;
      string yIDfzPpHAV;
      string SfwNMnSqZM;
      string jPYldFapQC;
      string mDKZGhZEhu;
      string oTlVcENNet;
      string dLYfyhjRCt;
      string nAqyhPUVGd;
      string ONJeXPdCJZ;
      if(nSTPiizAeo == qdkhKebmru){qNKWYadebY = true;}
      else if(qdkhKebmru == nSTPiizAeo){HlkskcbgUw = true;}
      if(qujarQAHyg == jJbqhwDzrh){yHCmGncnnM = true;}
      else if(jJbqhwDzrh == qujarQAHyg){DhmSErOywF = true;}
      if(IVEYUhiWPu == yIDfzPpHAV){iqGBePCtHi = true;}
      else if(yIDfzPpHAV == IVEYUhiWPu){wmlgKQgxzZ = true;}
      if(uoVyfcxcAy == SfwNMnSqZM){EhIcCapTZG = true;}
      else if(SfwNMnSqZM == uoVyfcxcAy){ZtKGRSubmT = true;}
      if(UmmBGQPfxd == jPYldFapQC){kpthfGLVYi = true;}
      else if(jPYldFapQC == UmmBGQPfxd){mlrpzlsJMp = true;}
      if(DjlFQSNtuA == mDKZGhZEhu){lpOwpAGIqI = true;}
      else if(mDKZGhZEhu == DjlFQSNtuA){lUFISUzEiT = true;}
      if(AckjsBZktO == oTlVcENNet){YjWrTPzYje = true;}
      else if(oTlVcENNet == AckjsBZktO){wdhtWIXgAl = true;}
      if(SXHzMUhDSD == dLYfyhjRCt){yJNgbswXVY = true;}
      if(SDYkrLDQgN == nAqyhPUVGd){EfUYwOTMmC = true;}
      if(OCgzyqCCHE == ONJeXPdCJZ){UgbQhBSBNP = true;}
      while(dLYfyhjRCt == SXHzMUhDSD){xlKcSJtwhX = true;}
      while(nAqyhPUVGd == nAqyhPUVGd){XWkImFoBmx = true;}
      while(ONJeXPdCJZ == ONJeXPdCJZ){SpsSWIExTq = true;}
      if(qNKWYadebY == true){qNKWYadebY = false;}
      if(yHCmGncnnM == true){yHCmGncnnM = false;}
      if(iqGBePCtHi == true){iqGBePCtHi = false;}
      if(EhIcCapTZG == true){EhIcCapTZG = false;}
      if(kpthfGLVYi == true){kpthfGLVYi = false;}
      if(lpOwpAGIqI == true){lpOwpAGIqI = false;}
      if(YjWrTPzYje == true){YjWrTPzYje = false;}
      if(yJNgbswXVY == true){yJNgbswXVY = false;}
      if(EfUYwOTMmC == true){EfUYwOTMmC = false;}
      if(UgbQhBSBNP == true){UgbQhBSBNP = false;}
      if(HlkskcbgUw == true){HlkskcbgUw = false;}
      if(DhmSErOywF == true){DhmSErOywF = false;}
      if(wmlgKQgxzZ == true){wmlgKQgxzZ = false;}
      if(ZtKGRSubmT == true){ZtKGRSubmT = false;}
      if(mlrpzlsJMp == true){mlrpzlsJMp = false;}
      if(lUFISUzEiT == true){lUFISUzEiT = false;}
      if(wdhtWIXgAl == true){wdhtWIXgAl = false;}
      if(xlKcSJtwhX == true){xlKcSJtwhX = false;}
      if(XWkImFoBmx == true){XWkImFoBmx = false;}
      if(SpsSWIExTq == true){SpsSWIExTq = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class THOHOEBVQO
{ 
  void WBLsiUTDHI()
  { 
      bool CODRVVXsTS = false;
      bool gUaetwoImg = false;
      bool LefzHkKyiZ = false;
      bool mFATRnEULJ = false;
      bool kwesbnakno = false;
      bool lHcaGWhDKk = false;
      bool KYJtfbUGMF = false;
      bool rTwcwOAlXT = false;
      bool JYKtmebYMu = false;
      bool FAKayLljNz = false;
      bool TTXixSLzeq = false;
      bool WueqakOPGA = false;
      bool zutRzqIVej = false;
      bool cPYXzpAawT = false;
      bool TctVXJUSzM = false;
      bool wVdMAXZGiF = false;
      bool JpUtOGWGqk = false;
      bool jmTlgWBXtp = false;
      bool WoVRmGlLXb = false;
      bool PtrpJdHzGR = false;
      string RdowENKqMW;
      string SeEqKYTctT;
      string CfhuJZdApm;
      string NVurLcdTSs;
      string ATAkdqzxiC;
      string YJhPmlwsme;
      string iKBZJxjMeh;
      string uAKBThfxQS;
      string ZgRmNDmeEZ;
      string fjgnKpSabg;
      string HlRBHXzgwF;
      string GzcTEBrNLF;
      string JEIFBKTWpc;
      string OFUjLmOQfp;
      string DGizSgHMiI;
      string xFOVWCdMzx;
      string DbpSpBdsIQ;
      string mbgVBDVIlU;
      string CHmhfJpzMI;
      string JBQilbPPIR;
      if(RdowENKqMW == HlRBHXzgwF){CODRVVXsTS = true;}
      else if(HlRBHXzgwF == RdowENKqMW){TTXixSLzeq = true;}
      if(SeEqKYTctT == GzcTEBrNLF){gUaetwoImg = true;}
      else if(GzcTEBrNLF == SeEqKYTctT){WueqakOPGA = true;}
      if(CfhuJZdApm == JEIFBKTWpc){LefzHkKyiZ = true;}
      else if(JEIFBKTWpc == CfhuJZdApm){zutRzqIVej = true;}
      if(NVurLcdTSs == OFUjLmOQfp){mFATRnEULJ = true;}
      else if(OFUjLmOQfp == NVurLcdTSs){cPYXzpAawT = true;}
      if(ATAkdqzxiC == DGizSgHMiI){kwesbnakno = true;}
      else if(DGizSgHMiI == ATAkdqzxiC){TctVXJUSzM = true;}
      if(YJhPmlwsme == xFOVWCdMzx){lHcaGWhDKk = true;}
      else if(xFOVWCdMzx == YJhPmlwsme){wVdMAXZGiF = true;}
      if(iKBZJxjMeh == DbpSpBdsIQ){KYJtfbUGMF = true;}
      else if(DbpSpBdsIQ == iKBZJxjMeh){JpUtOGWGqk = true;}
      if(uAKBThfxQS == mbgVBDVIlU){rTwcwOAlXT = true;}
      if(ZgRmNDmeEZ == CHmhfJpzMI){JYKtmebYMu = true;}
      if(fjgnKpSabg == JBQilbPPIR){FAKayLljNz = true;}
      while(mbgVBDVIlU == uAKBThfxQS){jmTlgWBXtp = true;}
      while(CHmhfJpzMI == CHmhfJpzMI){WoVRmGlLXb = true;}
      while(JBQilbPPIR == JBQilbPPIR){PtrpJdHzGR = true;}
      if(CODRVVXsTS == true){CODRVVXsTS = false;}
      if(gUaetwoImg == true){gUaetwoImg = false;}
      if(LefzHkKyiZ == true){LefzHkKyiZ = false;}
      if(mFATRnEULJ == true){mFATRnEULJ = false;}
      if(kwesbnakno == true){kwesbnakno = false;}
      if(lHcaGWhDKk == true){lHcaGWhDKk = false;}
      if(KYJtfbUGMF == true){KYJtfbUGMF = false;}
      if(rTwcwOAlXT == true){rTwcwOAlXT = false;}
      if(JYKtmebYMu == true){JYKtmebYMu = false;}
      if(FAKayLljNz == true){FAKayLljNz = false;}
      if(TTXixSLzeq == true){TTXixSLzeq = false;}
      if(WueqakOPGA == true){WueqakOPGA = false;}
      if(zutRzqIVej == true){zutRzqIVej = false;}
      if(cPYXzpAawT == true){cPYXzpAawT = false;}
      if(TctVXJUSzM == true){TctVXJUSzM = false;}
      if(wVdMAXZGiF == true){wVdMAXZGiF = false;}
      if(JpUtOGWGqk == true){JpUtOGWGqk = false;}
      if(jmTlgWBXtp == true){jmTlgWBXtp = false;}
      if(WoVRmGlLXb == true){WoVRmGlLXb = false;}
      if(PtrpJdHzGR == true){PtrpJdHzGR = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class VBETQSEBVQ
{ 
  void dTocnBRHls()
  { 
      bool cncchtiCrV = false;
      bool zScFwGTGaY = false;
      bool tFXuJieYwp = false;
      bool OftGxFKeUe = false;
      bool laGLqQoMiV = false;
      bool PUxiXXPgTy = false;
      bool PJGBHSYWHq = false;
      bool wolulTyMEr = false;
      bool tRoZZthKhZ = false;
      bool ocKVPcUFEX = false;
      bool rSFrZahCxE = false;
      bool YDcaExTfaM = false;
      bool kNnhBSdQfJ = false;
      bool zsHfAbMwAz = false;
      bool LfNdAlAiQR = false;
      bool IndqlddWqG = false;
      bool wRVmFeuwac = false;
      bool kWfaBFAJgO = false;
      bool SuAOBLPPsV = false;
      bool xoFsCtJaYV = false;
      string VULqsHOdah;
      string dorKCMCDNb;
      string FPdcgOthHL;
      string cVSgZsdFAT;
      string UilJuEjEwK;
      string sJoZaFpiJd;
      string MgVOEHWKkJ;
      string hxCsGWcUhc;
      string eokwSSgrGT;
      string CuuWHnMXEm;
      string hZTmbuHuSD;
      string cUAgzWuYwl;
      string adEQHmBelS;
      string JFQttFVyUW;
      string LkYCqgtOWj;
      string rnbRMDUDeo;
      string tbJAtXEGlt;
      string BxEYZpEBZZ;
      string smHCZVcyhp;
      string jSSecjaYHS;
      if(VULqsHOdah == hZTmbuHuSD){cncchtiCrV = true;}
      else if(hZTmbuHuSD == VULqsHOdah){rSFrZahCxE = true;}
      if(dorKCMCDNb == cUAgzWuYwl){zScFwGTGaY = true;}
      else if(cUAgzWuYwl == dorKCMCDNb){YDcaExTfaM = true;}
      if(FPdcgOthHL == adEQHmBelS){tFXuJieYwp = true;}
      else if(adEQHmBelS == FPdcgOthHL){kNnhBSdQfJ = true;}
      if(cVSgZsdFAT == JFQttFVyUW){OftGxFKeUe = true;}
      else if(JFQttFVyUW == cVSgZsdFAT){zsHfAbMwAz = true;}
      if(UilJuEjEwK == LkYCqgtOWj){laGLqQoMiV = true;}
      else if(LkYCqgtOWj == UilJuEjEwK){LfNdAlAiQR = true;}
      if(sJoZaFpiJd == rnbRMDUDeo){PUxiXXPgTy = true;}
      else if(rnbRMDUDeo == sJoZaFpiJd){IndqlddWqG = true;}
      if(MgVOEHWKkJ == tbJAtXEGlt){PJGBHSYWHq = true;}
      else if(tbJAtXEGlt == MgVOEHWKkJ){wRVmFeuwac = true;}
      if(hxCsGWcUhc == BxEYZpEBZZ){wolulTyMEr = true;}
      if(eokwSSgrGT == smHCZVcyhp){tRoZZthKhZ = true;}
      if(CuuWHnMXEm == jSSecjaYHS){ocKVPcUFEX = true;}
      while(BxEYZpEBZZ == hxCsGWcUhc){kWfaBFAJgO = true;}
      while(smHCZVcyhp == smHCZVcyhp){SuAOBLPPsV = true;}
      while(jSSecjaYHS == jSSecjaYHS){xoFsCtJaYV = true;}
      if(cncchtiCrV == true){cncchtiCrV = false;}
      if(zScFwGTGaY == true){zScFwGTGaY = false;}
      if(tFXuJieYwp == true){tFXuJieYwp = false;}
      if(OftGxFKeUe == true){OftGxFKeUe = false;}
      if(laGLqQoMiV == true){laGLqQoMiV = false;}
      if(PUxiXXPgTy == true){PUxiXXPgTy = false;}
      if(PJGBHSYWHq == true){PJGBHSYWHq = false;}
      if(wolulTyMEr == true){wolulTyMEr = false;}
      if(tRoZZthKhZ == true){tRoZZthKhZ = false;}
      if(ocKVPcUFEX == true){ocKVPcUFEX = false;}
      if(rSFrZahCxE == true){rSFrZahCxE = false;}
      if(YDcaExTfaM == true){YDcaExTfaM = false;}
      if(kNnhBSdQfJ == true){kNnhBSdQfJ = false;}
      if(zsHfAbMwAz == true){zsHfAbMwAz = false;}
      if(LfNdAlAiQR == true){LfNdAlAiQR = false;}
      if(IndqlddWqG == true){IndqlddWqG = false;}
      if(wRVmFeuwac == true){wRVmFeuwac = false;}
      if(kWfaBFAJgO == true){kWfaBFAJgO = false;}
      if(SuAOBLPPsV == true){SuAOBLPPsV = false;}
      if(xoFsCtJaYV == true){xoFsCtJaYV = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ATKCAYGTGA
{ 
  void hVUnInhFDH()
  { 
      bool hAUpWKaKMZ = false;
      bool hcpSeusjmI = false;
      bool mTNpIWzGVc = false;
      bool YWdMNGXJIa = false;
      bool xLjWhqmLuP = false;
      bool cAUOLNldhz = false;
      bool LUoiBKzdEX = false;
      bool WjbMLbjqIQ = false;
      bool jnKFzXCOBA = false;
      bool TsbIObVAVW = false;
      bool rJGWssYIDi = false;
      bool duVQPXJjjd = false;
      bool IjcEDdISxi = false;
      bool GiVkYagnMG = false;
      bool ysKRWYWLsq = false;
      bool SkQrOHgCSO = false;
      bool kGKTcuYjqn = false;
      bool pVXceOYKgd = false;
      bool GwYFQURdWQ = false;
      bool teUxUqipob = false;
      string KeaFxZHBft;
      string iFXxOxXQHR;
      string mzNFTmjirr;
      string ABNDsrZtEw;
      string hIMYcREdfb;
      string kXehoLQVkJ;
      string oYIntgrTEW;
      string WcMKidmocC;
      string iGCQHcVfOh;
      string aEJtGJzYeu;
      string ZjYJEHLTyK;
      string XbrhaiVrfQ;
      string LMFVuSTSEZ;
      string eOKgcygrLz;
      string QBgDJzLaSX;
      string nGrNjMdDOl;
      string nsPYfJRLFc;
      string BcfgAJeBMw;
      string GszfkXVxxP;
      string TddZLWkIXz;
      if(KeaFxZHBft == ZjYJEHLTyK){hAUpWKaKMZ = true;}
      else if(ZjYJEHLTyK == KeaFxZHBft){rJGWssYIDi = true;}
      if(iFXxOxXQHR == XbrhaiVrfQ){hcpSeusjmI = true;}
      else if(XbrhaiVrfQ == iFXxOxXQHR){duVQPXJjjd = true;}
      if(mzNFTmjirr == LMFVuSTSEZ){mTNpIWzGVc = true;}
      else if(LMFVuSTSEZ == mzNFTmjirr){IjcEDdISxi = true;}
      if(ABNDsrZtEw == eOKgcygrLz){YWdMNGXJIa = true;}
      else if(eOKgcygrLz == ABNDsrZtEw){GiVkYagnMG = true;}
      if(hIMYcREdfb == QBgDJzLaSX){xLjWhqmLuP = true;}
      else if(QBgDJzLaSX == hIMYcREdfb){ysKRWYWLsq = true;}
      if(kXehoLQVkJ == nGrNjMdDOl){cAUOLNldhz = true;}
      else if(nGrNjMdDOl == kXehoLQVkJ){SkQrOHgCSO = true;}
      if(oYIntgrTEW == nsPYfJRLFc){LUoiBKzdEX = true;}
      else if(nsPYfJRLFc == oYIntgrTEW){kGKTcuYjqn = true;}
      if(WcMKidmocC == BcfgAJeBMw){WjbMLbjqIQ = true;}
      if(iGCQHcVfOh == GszfkXVxxP){jnKFzXCOBA = true;}
      if(aEJtGJzYeu == TddZLWkIXz){TsbIObVAVW = true;}
      while(BcfgAJeBMw == WcMKidmocC){pVXceOYKgd = true;}
      while(GszfkXVxxP == GszfkXVxxP){GwYFQURdWQ = true;}
      while(TddZLWkIXz == TddZLWkIXz){teUxUqipob = true;}
      if(hAUpWKaKMZ == true){hAUpWKaKMZ = false;}
      if(hcpSeusjmI == true){hcpSeusjmI = false;}
      if(mTNpIWzGVc == true){mTNpIWzGVc = false;}
      if(YWdMNGXJIa == true){YWdMNGXJIa = false;}
      if(xLjWhqmLuP == true){xLjWhqmLuP = false;}
      if(cAUOLNldhz == true){cAUOLNldhz = false;}
      if(LUoiBKzdEX == true){LUoiBKzdEX = false;}
      if(WjbMLbjqIQ == true){WjbMLbjqIQ = false;}
      if(jnKFzXCOBA == true){jnKFzXCOBA = false;}
      if(TsbIObVAVW == true){TsbIObVAVW = false;}
      if(rJGWssYIDi == true){rJGWssYIDi = false;}
      if(duVQPXJjjd == true){duVQPXJjjd = false;}
      if(IjcEDdISxi == true){IjcEDdISxi = false;}
      if(GiVkYagnMG == true){GiVkYagnMG = false;}
      if(ysKRWYWLsq == true){ysKRWYWLsq = false;}
      if(SkQrOHgCSO == true){SkQrOHgCSO = false;}
      if(kGKTcuYjqn == true){kGKTcuYjqn = false;}
      if(pVXceOYKgd == true){pVXceOYKgd = false;}
      if(GwYFQURdWQ == true){GwYFQURdWQ = false;}
      if(teUxUqipob == true){teUxUqipob = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ZYWJGWTGZO
{ 
  void iKhGZrkIKq()
  { 
      bool dPkyiETore = false;
      bool nmoOywMwEz = false;
      bool jTNOeQPUgN = false;
      bool wxWIkLhOWf = false;
      bool kSrtORhTYE = false;
      bool HctrrJBVTR = false;
      bool oyHehpPgcm = false;
      bool OxYXORbSAM = false;
      bool hwwcGTCeQb = false;
      bool JLuFqlKNMX = false;
      bool BBplpJDWca = false;
      bool hiVGirBeXO = false;
      bool njPBcDCTfb = false;
      bool HOGbLgFKmx = false;
      bool PfBosapnVy = false;
      bool keoxjQloIm = false;
      bool GipBTSbtuC = false;
      bool mqrayHhaHB = false;
      bool MOygqbZIqt = false;
      bool uQBzpgpAVa = false;
      string SqYiXsLxXE;
      string xxldcAuHfL;
      string QlLjZLUNDT;
      string KmwIbeERkm;
      string KwdxJTqkZA;
      string PrbepgenJL;
      string XxsEtKAdCy;
      string udVsBUHOZF;
      string UimDDRLQRu;
      string zQHZOMDZDz;
      string SNibFlWhcV;
      string ZQNEqoJpAc;
      string yiLHyeVwen;
      string QgLZlHzLxR;
      string tcnwViCDhY;
      string DMYZAiCbdZ;
      string PaDoErFmRj;
      string oyrihVuOlO;
      string bSNGkEHkZC;
      string EXhWBTZyqc;
      if(SqYiXsLxXE == SNibFlWhcV){dPkyiETore = true;}
      else if(SNibFlWhcV == SqYiXsLxXE){BBplpJDWca = true;}
      if(xxldcAuHfL == ZQNEqoJpAc){nmoOywMwEz = true;}
      else if(ZQNEqoJpAc == xxldcAuHfL){hiVGirBeXO = true;}
      if(QlLjZLUNDT == yiLHyeVwen){jTNOeQPUgN = true;}
      else if(yiLHyeVwen == QlLjZLUNDT){njPBcDCTfb = true;}
      if(KmwIbeERkm == QgLZlHzLxR){wxWIkLhOWf = true;}
      else if(QgLZlHzLxR == KmwIbeERkm){HOGbLgFKmx = true;}
      if(KwdxJTqkZA == tcnwViCDhY){kSrtORhTYE = true;}
      else if(tcnwViCDhY == KwdxJTqkZA){PfBosapnVy = true;}
      if(PrbepgenJL == DMYZAiCbdZ){HctrrJBVTR = true;}
      else if(DMYZAiCbdZ == PrbepgenJL){keoxjQloIm = true;}
      if(XxsEtKAdCy == PaDoErFmRj){oyHehpPgcm = true;}
      else if(PaDoErFmRj == XxsEtKAdCy){GipBTSbtuC = true;}
      if(udVsBUHOZF == oyrihVuOlO){OxYXORbSAM = true;}
      if(UimDDRLQRu == bSNGkEHkZC){hwwcGTCeQb = true;}
      if(zQHZOMDZDz == EXhWBTZyqc){JLuFqlKNMX = true;}
      while(oyrihVuOlO == udVsBUHOZF){mqrayHhaHB = true;}
      while(bSNGkEHkZC == bSNGkEHkZC){MOygqbZIqt = true;}
      while(EXhWBTZyqc == EXhWBTZyqc){uQBzpgpAVa = true;}
      if(dPkyiETore == true){dPkyiETore = false;}
      if(nmoOywMwEz == true){nmoOywMwEz = false;}
      if(jTNOeQPUgN == true){jTNOeQPUgN = false;}
      if(wxWIkLhOWf == true){wxWIkLhOWf = false;}
      if(kSrtORhTYE == true){kSrtORhTYE = false;}
      if(HctrrJBVTR == true){HctrrJBVTR = false;}
      if(oyHehpPgcm == true){oyHehpPgcm = false;}
      if(OxYXORbSAM == true){OxYXORbSAM = false;}
      if(hwwcGTCeQb == true){hwwcGTCeQb = false;}
      if(JLuFqlKNMX == true){JLuFqlKNMX = false;}
      if(BBplpJDWca == true){BBplpJDWca = false;}
      if(hiVGirBeXO == true){hiVGirBeXO = false;}
      if(njPBcDCTfb == true){njPBcDCTfb = false;}
      if(HOGbLgFKmx == true){HOGbLgFKmx = false;}
      if(PfBosapnVy == true){PfBosapnVy = false;}
      if(keoxjQloIm == true){keoxjQloIm = false;}
      if(GipBTSbtuC == true){GipBTSbtuC = false;}
      if(mqrayHhaHB == true){mqrayHhaHB = false;}
      if(MOygqbZIqt == true){MOygqbZIqt = false;}
      if(uQBzpgpAVa == true){uQBzpgpAVa = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class KABLJGNJYI
{ 
  void LqbZFANsYI()
  { 
      bool pUrepuLpTD = false;
      bool KyPzqOidhX = false;
      bool ScQxfSFxnD = false;
      bool lmGMyQWqxA = false;
      bool BCjYmupudh = false;
      bool GdlYxMEtyX = false;
      bool oOBPRXrDBZ = false;
      bool qqggWQsqJk = false;
      bool bKTGxHAgOk = false;
      bool McsZLlBTHP = false;
      bool DmsDpOxECg = false;
      bool TNkMueutRK = false;
      bool WKnQNOxGVj = false;
      bool PYithnQPPi = false;
      bool KwegQyqgdf = false;
      bool qGCxrrybob = false;
      bool QpiRRudpdG = false;
      bool NeeizafHAO = false;
      bool WQcosIWlCp = false;
      bool pnanRTNMPU = false;
      string rBzYYqhryc;
      string ijUIRmnjLX;
      string eHjhdSPOwh;
      string MPMYskBpgR;
      string zrBRmsjOfR;
      string cjRsOkQQtF;
      string aTwPQTXwOf;
      string lxCtXmFSuk;
      string iAieFbxSKI;
      string UjwapfulBT;
      string VmAzeegWUb;
      string fxUJVZjRKn;
      string VElGLBBbEq;
      string qTQbOuAizZ;
      string uaxhcWOlFP;
      string wdaMyRBsnd;
      string dFSdkLgstZ;
      string ZASkstGDdn;
      string lOWnuMrDcx;
      string sGuUwoITxQ;
      if(rBzYYqhryc == VmAzeegWUb){pUrepuLpTD = true;}
      else if(VmAzeegWUb == rBzYYqhryc){DmsDpOxECg = true;}
      if(ijUIRmnjLX == fxUJVZjRKn){KyPzqOidhX = true;}
      else if(fxUJVZjRKn == ijUIRmnjLX){TNkMueutRK = true;}
      if(eHjhdSPOwh == VElGLBBbEq){ScQxfSFxnD = true;}
      else if(VElGLBBbEq == eHjhdSPOwh){WKnQNOxGVj = true;}
      if(MPMYskBpgR == qTQbOuAizZ){lmGMyQWqxA = true;}
      else if(qTQbOuAizZ == MPMYskBpgR){PYithnQPPi = true;}
      if(zrBRmsjOfR == uaxhcWOlFP){BCjYmupudh = true;}
      else if(uaxhcWOlFP == zrBRmsjOfR){KwegQyqgdf = true;}
      if(cjRsOkQQtF == wdaMyRBsnd){GdlYxMEtyX = true;}
      else if(wdaMyRBsnd == cjRsOkQQtF){qGCxrrybob = true;}
      if(aTwPQTXwOf == dFSdkLgstZ){oOBPRXrDBZ = true;}
      else if(dFSdkLgstZ == aTwPQTXwOf){QpiRRudpdG = true;}
      if(lxCtXmFSuk == ZASkstGDdn){qqggWQsqJk = true;}
      if(iAieFbxSKI == lOWnuMrDcx){bKTGxHAgOk = true;}
      if(UjwapfulBT == sGuUwoITxQ){McsZLlBTHP = true;}
      while(ZASkstGDdn == lxCtXmFSuk){NeeizafHAO = true;}
      while(lOWnuMrDcx == lOWnuMrDcx){WQcosIWlCp = true;}
      while(sGuUwoITxQ == sGuUwoITxQ){pnanRTNMPU = true;}
      if(pUrepuLpTD == true){pUrepuLpTD = false;}
      if(KyPzqOidhX == true){KyPzqOidhX = false;}
      if(ScQxfSFxnD == true){ScQxfSFxnD = false;}
      if(lmGMyQWqxA == true){lmGMyQWqxA = false;}
      if(BCjYmupudh == true){BCjYmupudh = false;}
      if(GdlYxMEtyX == true){GdlYxMEtyX = false;}
      if(oOBPRXrDBZ == true){oOBPRXrDBZ = false;}
      if(qqggWQsqJk == true){qqggWQsqJk = false;}
      if(bKTGxHAgOk == true){bKTGxHAgOk = false;}
      if(McsZLlBTHP == true){McsZLlBTHP = false;}
      if(DmsDpOxECg == true){DmsDpOxECg = false;}
      if(TNkMueutRK == true){TNkMueutRK = false;}
      if(WKnQNOxGVj == true){WKnQNOxGVj = false;}
      if(PYithnQPPi == true){PYithnQPPi = false;}
      if(KwegQyqgdf == true){KwegQyqgdf = false;}
      if(qGCxrrybob == true){qGCxrrybob = false;}
      if(QpiRRudpdG == true){QpiRRudpdG = false;}
      if(NeeizafHAO == true){NeeizafHAO = false;}
      if(WQcosIWlCp == true){WQcosIWlCp = false;}
      if(pnanRTNMPU == true){pnanRTNMPU = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class AUQKGCFEUS
{ 
  void mTPatCHfEA()
  { 
      bool JcHkaaJuPc = false;
      bool JzIeSZQYJd = false;
      bool dTefaqRIVx = false;
      bool YqTjxLpmyD = false;
      bool hZcChiTlbj = false;
      bool WztOwoyaKi = false;
      bool IAYRERfYgG = false;
      bool jdZGuDqbxq = false;
      bool XegkaZxEmH = false;
      bool NOcPthAKyQ = false;
      bool DuEUzsYnSr = false;
      bool yVcSLcTmqe = false;
      bool UakYsVAKLN = false;
      bool CfdgnbaLlg = false;
      bool AJluIrkKEQ = false;
      bool NNSSubJfbd = false;
      bool rNbEoSqwdi = false;
      bool CIBJdLopPA = false;
      bool jpGuAoMLkL = false;
      bool RujHnmggFp = false;
      string HyYwjgtPNM;
      string DYLIIOsISb;
      string qZdFjtLqRP;
      string MsVqCzbNoi;
      string wRnQEtJJYa;
      string faDFAgUTLo;
      string UDyyAJCTFI;
      string meSjfYlMBo;
      string jBSqikpQqU;
      string eHCzOsTpQt;
      string roEPmBITZa;
      string uISVESjHOO;
      string TlcjdecSUl;
      string EwuGxuxCDh;
      string cWqPZWoftu;
      string IMaCZFMTlI;
      string XoymWzMLYJ;
      string UhDflxuMKH;
      string oxXYdrUsfr;
      string icQuKnXDsa;
      if(HyYwjgtPNM == roEPmBITZa){JcHkaaJuPc = true;}
      else if(roEPmBITZa == HyYwjgtPNM){DuEUzsYnSr = true;}
      if(DYLIIOsISb == uISVESjHOO){JzIeSZQYJd = true;}
      else if(uISVESjHOO == DYLIIOsISb){yVcSLcTmqe = true;}
      if(qZdFjtLqRP == TlcjdecSUl){dTefaqRIVx = true;}
      else if(TlcjdecSUl == qZdFjtLqRP){UakYsVAKLN = true;}
      if(MsVqCzbNoi == EwuGxuxCDh){YqTjxLpmyD = true;}
      else if(EwuGxuxCDh == MsVqCzbNoi){CfdgnbaLlg = true;}
      if(wRnQEtJJYa == cWqPZWoftu){hZcChiTlbj = true;}
      else if(cWqPZWoftu == wRnQEtJJYa){AJluIrkKEQ = true;}
      if(faDFAgUTLo == IMaCZFMTlI){WztOwoyaKi = true;}
      else if(IMaCZFMTlI == faDFAgUTLo){NNSSubJfbd = true;}
      if(UDyyAJCTFI == XoymWzMLYJ){IAYRERfYgG = true;}
      else if(XoymWzMLYJ == UDyyAJCTFI){rNbEoSqwdi = true;}
      if(meSjfYlMBo == UhDflxuMKH){jdZGuDqbxq = true;}
      if(jBSqikpQqU == oxXYdrUsfr){XegkaZxEmH = true;}
      if(eHCzOsTpQt == icQuKnXDsa){NOcPthAKyQ = true;}
      while(UhDflxuMKH == meSjfYlMBo){CIBJdLopPA = true;}
      while(oxXYdrUsfr == oxXYdrUsfr){jpGuAoMLkL = true;}
      while(icQuKnXDsa == icQuKnXDsa){RujHnmggFp = true;}
      if(JcHkaaJuPc == true){JcHkaaJuPc = false;}
      if(JzIeSZQYJd == true){JzIeSZQYJd = false;}
      if(dTefaqRIVx == true){dTefaqRIVx = false;}
      if(YqTjxLpmyD == true){YqTjxLpmyD = false;}
      if(hZcChiTlbj == true){hZcChiTlbj = false;}
      if(WztOwoyaKi == true){WztOwoyaKi = false;}
      if(IAYRERfYgG == true){IAYRERfYgG = false;}
      if(jdZGuDqbxq == true){jdZGuDqbxq = false;}
      if(XegkaZxEmH == true){XegkaZxEmH = false;}
      if(NOcPthAKyQ == true){NOcPthAKyQ = false;}
      if(DuEUzsYnSr == true){DuEUzsYnSr = false;}
      if(yVcSLcTmqe == true){yVcSLcTmqe = false;}
      if(UakYsVAKLN == true){UakYsVAKLN = false;}
      if(CfdgnbaLlg == true){CfdgnbaLlg = false;}
      if(AJluIrkKEQ == true){AJluIrkKEQ = false;}
      if(NNSSubJfbd == true){NNSSubJfbd = false;}
      if(rNbEoSqwdi == true){rNbEoSqwdi = false;}
      if(CIBJdLopPA == true){CIBJdLopPA = false;}
      if(jpGuAoMLkL == true){jpGuAoMLkL = false;}
      if(RujHnmggFp == true){RujHnmggFp = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JFZXXTVDST
{ 
  void gWOljsZyzi()
  { 
      bool wlVyqmCzJZ = false;
      bool szoIDqdDQA = false;
      bool GOboIfLXJz = false;
      bool YdgSHuEpGf = false;
      bool PxBuiqxLeA = false;
      bool aGSjTgJfoL = false;
      bool adpJBPRVBI = false;
      bool mmwkkJNgGR = false;
      bool hAsJYSKxzZ = false;
      bool kYBuEkHqtT = false;
      bool YbJVNmKhyK = false;
      bool ScZXdakfkh = false;
      bool EUWaWNlAgz = false;
      bool BWllrGqWwk = false;
      bool IbynHIzdRd = false;
      bool AbGLOEEhzz = false;
      bool BJBzJwpmeX = false;
      bool iNjzZbWhTN = false;
      bool OHiCzZPZWn = false;
      bool BJWwjzayeL = false;
      string AfQOtQemid;
      string IFHOgCXsag;
      string agPEmtxzwe;
      string XqgrDlYRDS;
      string MVSyVNBSgd;
      string mtTpxSQLbz;
      string TfpEFRYPKt;
      string dFpITFORiY;
      string seSSrcWptj;
      string VMpDzgfOrD;
      string olIlAgELWq;
      string EzcCGoNXMc;
      string uhUZYQOlIm;
      string ZzZejHVgRK;
      string XLTCfVjnxK;
      string THBOIHQiMj;
      string SSCjREZyRz;
      string SfFBFiNRsD;
      string ganySRxKVc;
      string pmThrFJjka;
      if(AfQOtQemid == olIlAgELWq){wlVyqmCzJZ = true;}
      else if(olIlAgELWq == AfQOtQemid){YbJVNmKhyK = true;}
      if(IFHOgCXsag == EzcCGoNXMc){szoIDqdDQA = true;}
      else if(EzcCGoNXMc == IFHOgCXsag){ScZXdakfkh = true;}
      if(agPEmtxzwe == uhUZYQOlIm){GOboIfLXJz = true;}
      else if(uhUZYQOlIm == agPEmtxzwe){EUWaWNlAgz = true;}
      if(XqgrDlYRDS == ZzZejHVgRK){YdgSHuEpGf = true;}
      else if(ZzZejHVgRK == XqgrDlYRDS){BWllrGqWwk = true;}
      if(MVSyVNBSgd == XLTCfVjnxK){PxBuiqxLeA = true;}
      else if(XLTCfVjnxK == MVSyVNBSgd){IbynHIzdRd = true;}
      if(mtTpxSQLbz == THBOIHQiMj){aGSjTgJfoL = true;}
      else if(THBOIHQiMj == mtTpxSQLbz){AbGLOEEhzz = true;}
      if(TfpEFRYPKt == SSCjREZyRz){adpJBPRVBI = true;}
      else if(SSCjREZyRz == TfpEFRYPKt){BJBzJwpmeX = true;}
      if(dFpITFORiY == SfFBFiNRsD){mmwkkJNgGR = true;}
      if(seSSrcWptj == ganySRxKVc){hAsJYSKxzZ = true;}
      if(VMpDzgfOrD == pmThrFJjka){kYBuEkHqtT = true;}
      while(SfFBFiNRsD == dFpITFORiY){iNjzZbWhTN = true;}
      while(ganySRxKVc == ganySRxKVc){OHiCzZPZWn = true;}
      while(pmThrFJjka == pmThrFJjka){BJWwjzayeL = true;}
      if(wlVyqmCzJZ == true){wlVyqmCzJZ = false;}
      if(szoIDqdDQA == true){szoIDqdDQA = false;}
      if(GOboIfLXJz == true){GOboIfLXJz = false;}
      if(YdgSHuEpGf == true){YdgSHuEpGf = false;}
      if(PxBuiqxLeA == true){PxBuiqxLeA = false;}
      if(aGSjTgJfoL == true){aGSjTgJfoL = false;}
      if(adpJBPRVBI == true){adpJBPRVBI = false;}
      if(mmwkkJNgGR == true){mmwkkJNgGR = false;}
      if(hAsJYSKxzZ == true){hAsJYSKxzZ = false;}
      if(kYBuEkHqtT == true){kYBuEkHqtT = false;}
      if(YbJVNmKhyK == true){YbJVNmKhyK = false;}
      if(ScZXdakfkh == true){ScZXdakfkh = false;}
      if(EUWaWNlAgz == true){EUWaWNlAgz = false;}
      if(BWllrGqWwk == true){BWllrGqWwk = false;}
      if(IbynHIzdRd == true){IbynHIzdRd = false;}
      if(AbGLOEEhzz == true){AbGLOEEhzz = false;}
      if(BJBzJwpmeX == true){BJBzJwpmeX = false;}
      if(iNjzZbWhTN == true){iNjzZbWhTN = false;}
      if(OHiCzZPZWn == true){OHiCzZPZWn = false;}
      if(BJWwjzayeL == true){BJWwjzayeL = false;}
    } 
}; 

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class JGRNLINOHR
{ 
  void LMybyMFGiC()
  { 
      bool midecTbCgB = false;
      bool NxdxlQWVRl = false;
      bool VfsaKBPjpe = false;
      bool iwmVZnmIJO = false;
      bool ezaZcBeMxk = false;
      bool zritHGoCUa = false;
      bool DqLBRRKHWy = false;
      bool dNLCHWESyI = false;
      bool WCmFfqMrNS = false;
      bool yjmiKnnYHL = false;
      bool qWpyHkgfmk = false;
      bool CVELEAEFVa = false;
      bool ERsDzsZcqS = false;
      bool heLmRAMnyu = false;
      bool JgEcQcebeJ = false;
      bool ihDfGYdlzN = false;
      bool KEnTCXKRsE = false;
      bool nQHaRXNQmW = false;
      bool OyyhjAaefs = false;
      bool VankjFrkZr = false;
      string eQmLPkleFF;
      string fxBhuTSPJb;
      string LraukKWTnA;
      string CDYhJxrpUy;
      string IAZSyegExK;
      string XhuwJlorZE;
      string ORqECcYTYx;
      string actKTLRIwG;
      string dRVdcUHbXW;
      string IzPQjpIVQU;
      string FMfAjtnjbx;
      string UgwSawwNag;
      string VwuCoaMdac;
      string CwraAQlVsi;
      string fXzaCbDwom;
      string tutizJRdKJ;
      string gKJztXoURJ;
      string KaTrBxwTAN;
      string WQnmhHBIbq;
      string zIKjBVOAgd;
      if(eQmLPkleFF == FMfAjtnjbx){midecTbCgB = true;}
      else if(FMfAjtnjbx == eQmLPkleFF){qWpyHkgfmk = true;}
      if(fxBhuTSPJb == UgwSawwNag){NxdxlQWVRl = true;}
      else if(UgwSawwNag == fxBhuTSPJb){CVELEAEFVa = true;}
      if(LraukKWTnA == VwuCoaMdac){VfsaKBPjpe = true;}
      else if(VwuCoaMdac == LraukKWTnA){ERsDzsZcqS = true;}
      if(CDYhJxrpUy == CwraAQlVsi){iwmVZnmIJO = true;}
      else if(CwraAQlVsi == CDYhJxrpUy){heLmRAMnyu = true;}
      if(IAZSyegExK == fXzaCbDwom){ezaZcBeMxk = true;}
      else if(fXzaCbDwom == IAZSyegExK){JgEcQcebeJ = true;}
      if(XhuwJlorZE == tutizJRdKJ){zritHGoCUa = true;}
      else if(tutizJRdKJ == XhuwJlorZE){ihDfGYdlzN = true;}
      if(ORqECcYTYx == gKJztXoURJ){DqLBRRKHWy = true;}
      else if(gKJztXoURJ == ORqECcYTYx){KEnTCXKRsE = true;}
      if(actKTLRIwG == KaTrBxwTAN){dNLCHWESyI = true;}
      if(dRVdcUHbXW == WQnmhHBIbq){WCmFfqMrNS = true;}
      if(IzPQjpIVQU == zIKjBVOAgd){yjmiKnnYHL = true;}
      while(KaTrBxwTAN == actKTLRIwG){nQHaRXNQmW = true;}
      while(WQnmhHBIbq == WQnmhHBIbq){OyyhjAaefs = true;}
      while(zIKjBVOAgd == zIKjBVOAgd){VankjFrkZr = true;}
      if(midecTbCgB == true){midecTbCgB = false;}
      if(NxdxlQWVRl == true){NxdxlQWVRl = false;}
      if(VfsaKBPjpe == true){VfsaKBPjpe = false;}
      if(iwmVZnmIJO == true){iwmVZnmIJO = false;}
      if(ezaZcBeMxk == true){ezaZcBeMxk = false;}
      if(zritHGoCUa == true){zritHGoCUa = false;}
      if(DqLBRRKHWy == true){DqLBRRKHWy = false;}
      if(dNLCHWESyI == true){dNLCHWESyI = false;}
      if(WCmFfqMrNS == true){WCmFfqMrNS = false;}
      if(yjmiKnnYHL == true){yjmiKnnYHL = false;}
      if(qWpyHkgfmk == true){qWpyHkgfmk = false;}
      if(CVELEAEFVa == true){CVELEAEFVa = false;}
      if(ERsDzsZcqS == true){ERsDzsZcqS = false;}
      if(heLmRAMnyu == true){heLmRAMnyu = false;}
      if(JgEcQcebeJ == true){JgEcQcebeJ = false;}
      if(ihDfGYdlzN == true){ihDfGYdlzN = false;}
      if(KEnTCXKRsE == true){KEnTCXKRsE = false;}
      if(nQHaRXNQmW == true){nQHaRXNQmW = false;}
      if(OyyhjAaefs == true){OyyhjAaefs = false;}
      if(VankjFrkZr == true){VankjFrkZr = false;}
    } 
}; 
