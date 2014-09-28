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

#include <audioformat.h>

AudioFormat::AudioFormat()
    : m_sampleRate(-1)
    , m_format(AudioFormat::SignedInt16)
    , m_channelCount(-1)
{
}

AudioFormat::~AudioFormat()
{
}

void AudioFormat::setSampleRate(int rate)
{
    m_sampleRate = rate;
}

int AudioFormat::sampleRate() const
{
    return m_sampleRate;
}

void AudioFormat::setFormat(AudioFormat::Format format)
{
    m_format = format;
}

AudioFormat::Format AudioFormat::format() const
{
    return m_format;
}

void AudioFormat::setChannelCount(int count)
{
    m_channelCount = count;
}

int AudioFormat::channelCount() const
{
    return m_channelCount;
}
