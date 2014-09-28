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

#ifndef AUDIOTIMER_H
#define AUDIOTIMER_H

#include <QObject>

class AudioGrabber;

class AudioTimer : public QObject
{
    Q_OBJECT
public:
    explicit AudioTimer(AudioGrabber *grabber, QObject *parent = 0);

    int elapsed() const;
private:
    AudioGrabber *m_grabber;
};

#endif // AUDIOTIMER_H
