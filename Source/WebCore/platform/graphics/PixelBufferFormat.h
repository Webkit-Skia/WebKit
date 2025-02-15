/*
 * Copyright (C) 2021 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "AlphaPremultiplication.h"
#include "ColorSpace.h"
#include "PixelFormat.h"
#include <wtf/Forward.h>
#include <wtf/Optional.h>

namespace WebCore {

struct PixelBufferFormat {
    AlphaPremultiplication alphaFormat;
    PixelFormat pixelFormat;
    DestinationColorSpace colorSpace;

    template<class Encoder> void encode(Encoder&) const;
    template<class Decoder> static Optional<PixelBufferFormat> decode(Decoder&);
};

WEBCORE_EXPORT TextStream& operator<<(TextStream&, const PixelBufferFormat&);

template<class Encoder> void PixelBufferFormat::encode(Encoder& encoder) const
{
    encoder << alphaFormat << pixelFormat << colorSpace;
}

template<class Decoder> Optional<PixelBufferFormat> PixelBufferFormat::decode(Decoder& decoder)
{
    AlphaPremultiplication alphaFormat;
    if (!decoder.decode(alphaFormat))
        return WTF::nullopt;

    PixelFormat pixelFormat;
    if (!decoder.decode(pixelFormat))
        return WTF::nullopt;

    DestinationColorSpace colorSpace;
    if (!decoder.decode(colorSpace))
        return WTF::nullopt;

    return { { alphaFormat, pixelFormat, colorSpace } };
}

}
