/****************************************************************************
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
****************************************************************************/

#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

class AudioFormat {
public:
    enum Format {
        SignedInt8 = 0x1,
        SignedInt16 = 0x2,
        SignedInt24 = 0x4,
        SignedInt32 = 0x8,
        Float32 = 0x10,
        Float64 = 0x20
    };

    AudioFormat();
    virtual ~AudioFormat();

    void setSampleRate(int rate);
    int sampleRate() const;

    void setFormat(AudioFormat::Format format);
    AudioFormat::Format format() const;

    void setChannelCount(int count);
    int channelCount() const;

private:
    int m_sampleRate;
    AudioFormat::Format m_format;
    int m_channelCount;
};
#endif
