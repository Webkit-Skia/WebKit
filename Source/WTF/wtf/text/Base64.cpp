/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2006 Alexey Proskuryakov <ap@webkit.org>
   Copyright (C) 2007-2021 Apple Inc. All rights reserved.
   Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   This code is based on the java implementation in HTTPClient
   package by Ronald Tschalär Copyright (C) 1996-1999.
*/

#include "config.h"
#include <wtf/text/Base64.h>

#include <limits.h>
#include <wtf/Optional.h>

namespace WTF {

constexpr const char nonAlphabet = -1;

constexpr unsigned encodeMapSize = 64;
constexpr unsigned decodeMapSize = 128;

static const char base64EncMap[encodeMapSize] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
    0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
};

static const char base64DecMap[decodeMapSize] = {
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, 0x3E, nonAlphabet, nonAlphabet, nonAlphabet, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    0x31, 0x32, 0x33, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet
};

static const char base64URLEncMap[encodeMapSize] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
    0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2D, 0x5F
};

static const char base64URLDecMap[decodeMapSize] = {
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, 0x3E, nonAlphabet, nonAlphabet,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet,
    nonAlphabet, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, 0x3F,
    nonAlphabet, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    0x31, 0x32, 0x33, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet, nonAlphabet
};

template<typename CharacterType> static void base64EncodeInternal(const uint8_t* inputDataBuffer, unsigned inputLength, CharacterType* destinationDataBuffer, unsigned destinationLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    ASSERT(destinationLength > 0);
    ASSERT(calculateBase64EncodedSize(inputLength, policy) == destinationLength);

    auto encodeMap = (map == Base64EncodeMap::URL) ? base64URLEncMap : base64EncMap;

    unsigned sidx = 0;
    unsigned didx = 0;
    unsigned count = 0;

    bool insertLFs = (policy == Base64EncodePolicy::InsertLFs && destinationLength > maximumBase64LineLengthWhenInsertingLFs);

    if (inputLength > 1) {
        while (sidx < inputLength - 2) {
            if (insertLFs) {
                if (count && !(count % maximumBase64LineLengthWhenInsertingLFs))
                    destinationDataBuffer[didx++] = '\n';
                count += 4;
            }

            destinationDataBuffer[didx++] = encodeMap[ (inputDataBuffer[sidx    ] >> 2) & 077];
            destinationDataBuffer[didx++] = encodeMap[((inputDataBuffer[sidx + 1] >> 4) & 017) | ((inputDataBuffer[sidx    ] << 4) & 077)];
            destinationDataBuffer[didx++] = encodeMap[((inputDataBuffer[sidx + 2] >> 6) & 003) | ((inputDataBuffer[sidx + 1] << 2) & 077)];
            destinationDataBuffer[didx++] = encodeMap[  inputDataBuffer[sidx + 2]       & 077];
            sidx += 3;
        }
    }

    if (sidx < inputLength) {
        if (insertLFs && (count > 0) && !(count % maximumBase64LineLengthWhenInsertingLFs))
            destinationDataBuffer[didx++] = '\n';

        destinationDataBuffer[didx++] = encodeMap[(inputDataBuffer[sidx] >> 2) & 077];
        if (sidx < inputLength - 1) {
            destinationDataBuffer[didx++] = encodeMap[((inputDataBuffer[sidx + 1] >> 4) & 017) | ((inputDataBuffer[sidx] << 4) & 077)];
            destinationDataBuffer[didx++] = encodeMap[ (inputDataBuffer[sidx + 1] << 2) & 077];
        } else
            destinationDataBuffer[didx++] = encodeMap[ (inputDataBuffer[sidx    ] << 4) & 077];
    }

    ASSERT(policy != Base64EncodePolicy::URL || didx == destinationLength);

    while (didx < destinationLength)
        destinationDataBuffer[didx++] = '=';
}

static Vector<uint8_t> base64EncodeInternal(const uint8_t* inputDataBuffer, unsigned inputLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    auto destinationLength = calculateBase64EncodedSize(inputLength, policy);
    if (!destinationLength)
        return { };

    Vector<uint8_t> destinationVector(destinationLength);
    base64EncodeInternal(inputDataBuffer, inputLength, destinationVector.data(), destinationLength, policy, map);
    return destinationVector;
}

void base64Encode(const void* inputDataBuffer, unsigned inputLength, UChar* destinationDataBuffer, unsigned destinationLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    if (!destinationLength)
        return;
    base64EncodeInternal(static_cast<const uint8_t*>(inputDataBuffer), inputLength, destinationDataBuffer, destinationLength, policy, map);
}

void base64Encode(const void* inputDataBuffer, unsigned inputLength, LChar* destinationDataBuffer, unsigned destinationLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    if (!destinationLength)
        return;
    base64EncodeInternal(static_cast<const uint8_t*>(inputDataBuffer), inputLength, destinationDataBuffer, destinationLength, policy, map);
}

Vector<uint8_t> base64EncodeToVector(const void* inputDataBuffer, unsigned inputLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    return base64EncodeInternal(static_cast<const uint8_t*>(inputDataBuffer), inputLength, policy, map);
}

String base64EncodeToString(const void* inputDataBuffer, unsigned inputLength, Base64EncodePolicy policy, Base64EncodeMap map)
{
    return makeString(base64Encoded(inputDataBuffer, inputLength, policy, map));
}

template<typename T> static Optional<Vector<uint8_t>> base64DecodeInternal(const T* inputDataBuffer, unsigned inputLength, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    if (!inputLength)
        return Vector<uint8_t> { };

    auto decodeMap = (map == Base64DecodeMap::URL) ? base64URLDecMap : base64DecMap;

    Vector<uint8_t> destination(inputLength);

    unsigned equalsSignCount = 0;
    unsigned destinationLength = 0;
    for (unsigned idx = 0; idx < inputLength; ++idx) {
        unsigned ch = inputDataBuffer[idx];
        if (ch == '=') {
            ++equalsSignCount;
            // There should never be more than 2 padding characters.
            if (options.contains(Base64DecodeOptions::ValidatePadding) && equalsSignCount > 2) {
                return WTF::nullopt;
            }
        } else {
            char decodedCharacter = ch < decodeMapSize ? decodeMap[ch] : nonAlphabet;
            if (decodedCharacter != nonAlphabet) {
                if (equalsSignCount)
                    return WTF::nullopt;
                destination[destinationLength++] = decodedCharacter;
            } else if (!options.contains(Base64DecodeOptions::IgnoreSpacesAndNewLines) || (!isLatin1(ch) || !isASCIISpace(ch) || (options.contains(Base64DecodeOptions::DiscardVerticalTab) && ch == '\v'))) {
                return WTF::nullopt;
            }
        }
    }

    // Make sure we shrink back the Vector before returning. destinationLength may be shorter than expected
    // in case of error or in case of ignored spaces.
    if (destinationLength < destination.size())
        destination.shrink(destinationLength);

    if (!destinationLength) {
        if (equalsSignCount)
            return WTF::nullopt;
        return Vector<uint8_t> { };
    }

    // The should be no padding if length is a multiple of 4.
    // We use (destinationLength + equalsSignCount) instead of length because we don't want to account for ignored characters (i.e. spaces).
    if (options.contains(Base64DecodeOptions::ValidatePadding) && equalsSignCount && (destinationLength + equalsSignCount) % 4)
        return WTF::nullopt;

    // Valid data is (n * 4 + [0,2,3]) characters long.
    if ((destinationLength % 4) == 1)
        return WTF::nullopt;
    
    // 4-byte to 3-byte conversion
    destinationLength -= (destinationLength + 3) / 4;
    if (!destinationLength)
        return WTF::nullopt;

    unsigned sidx = 0;
    unsigned didx = 0;
    if (destinationLength > 1) {
        while (didx < destinationLength - 2) {
            destination[didx    ] = (((destination[sidx    ] << 2) & 255) | ((destination[sidx + 1] >> 4) & 003));
            destination[didx + 1] = (((destination[sidx + 1] << 4) & 255) | ((destination[sidx + 2] >> 2) & 017));
            destination[didx + 2] = (((destination[sidx + 2] << 6) & 255) |  (destination[sidx + 3]       & 077));
            sidx += 4;
            didx += 3;
        }
    }

    if (didx < destinationLength)
        destination[didx] = (((destination[sidx    ] << 2) & 255) | ((destination[sidx + 1] >> 4) & 003));

    if (++didx < destinationLength)
        destination[didx] = (((destination[sidx + 1] << 4) & 255) | ((destination[sidx + 2] >> 2) & 017));

    if (destinationLength < destination.size())
        destination.shrink(destinationLength);
    return destination;
}

Optional<Vector<uint8_t>> base64Decode(const String& input, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    unsigned length = input.length();
    if (!length || input.is8Bit())
        return base64DecodeInternal(input.characters8(), length, options, map);
    return base64DecodeInternal(input.characters16(), length, options, map);
}

Optional<Vector<uint8_t>> base64Decode(StringView input, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    unsigned length = input.length();
    if (!length || input.is8Bit())
        return base64DecodeInternal(input.characters8(), length, options, map);
    return base64DecodeInternal(input.characters16(), length, options, map);
}

Optional<Vector<uint8_t>> base64Decode(const Vector<uint8_t>& input, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    if (input.size() > std::numeric_limits<unsigned>::max())
        return WTF::nullopt;
    return base64DecodeInternal(input.data(), input.size(), options, map);
}

Optional<Vector<uint8_t>> base64Decode(const Vector<char>& input, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    if (input.size() > std::numeric_limits<unsigned>::max())
        return WTF::nullopt;
    return base64DecodeInternal(reinterpret_cast<const uint8_t*>(input.data()), input.size(), options, map);
}

Optional<Vector<uint8_t>> base64Decode(const uint8_t* data, unsigned length, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    return base64DecodeInternal(data, length, options, map);
}

Optional<Vector<uint8_t>> base64Decode(const char* data, unsigned length, OptionSet<Base64DecodeOptions> options, Base64DecodeMap map)
{
    return base64DecodeInternal(reinterpret_cast<const LChar*>(data), length, options, map);
}

Optional<Vector<uint8_t>> base64URLDecode(const String& input)
{
    unsigned length = input.length();
    if (!length || input.is8Bit())
        return base64DecodeInternal(input.characters8(), length, { }, Base64DecodeMap::URL);
    return base64DecodeInternal(input.characters16(), length, { }, Base64DecodeMap::URL);
}

Optional<Vector<uint8_t>> base64URLDecode(StringView input)
{
    unsigned length = input.length();
    if (!length || input.is8Bit())
        return base64DecodeInternal(input.characters8(), length, { }, Base64DecodeMap::URL);
    return base64DecodeInternal(input.characters16(), length, { }, Base64DecodeMap::URL);
}

Optional<Vector<uint8_t>> base64URLDecode(const Vector<uint8_t>& input)
{
    if (input.size() > std::numeric_limits<unsigned>::max())
        return WTF::nullopt;
    return base64DecodeInternal(input.data(), input.size(), { }, Base64DecodeMap::URL);
}

Optional<Vector<uint8_t>> base64URLDecode(const Vector<char>& input)
{
    if (input.size() > std::numeric_limits<unsigned>::max())
        return WTF::nullopt;
    return base64DecodeInternal(reinterpret_cast<const uint8_t*>(input.data()), input.size(), { }, Base64DecodeMap::URL);
}

Optional<Vector<uint8_t>> base64URLDecode(const uint8_t* data, unsigned length)
{
    return base64DecodeInternal(data, length, { }, Base64DecodeMap::URL);
}

Optional<Vector<uint8_t>> base64URLDecode(const char* data, unsigned length)
{
    return base64DecodeInternal(reinterpret_cast<const uint8_t*>(data), length, { }, Base64DecodeMap::URL);
}

} // namespace WTF
