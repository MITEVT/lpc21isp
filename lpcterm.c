/******************************************************************************

Project:           Portable command line ISP for NXP LPC1000 / LPC2000 family
                   and Analog Devices ADUC70xx

Filename:          lpcterm.c

Compiler:          Microsoft VC 6/7, Microsoft VS2008, Microsoft VS2010,
                   GCC Cygwin, GCC Linux, GCC ARM ELF

Author:            Martin Maurer (Martin.Maurer@clibb.de)

Copyright:         (c) Martin Maurer 2003-2011, All rights reserved
Portions Copyright (c) by Aeolus Development 2004 http://www.aeolusdevelopment.com

    This file is part of lpc21isp.

    lpc21isp is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    lpc21isp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    and GNU General Public License along with lpc21isp.
    If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(_WIN32)
#if !defined __BORLANDC__
#include "StdAfx.h"
#endif
#endif // defined(_WIN32)
#include "lpc21isp.h"
#include "lpcterm.h"
#include "errors.h"

#ifdef TERMINAL_SUPPORT
/***************************** Terminal *********************************/
/**  Acts as a simple dumb terminal. Press 'ESC' to exit.
*/
BOOL CheckTerminalParameters(ISP_ENVIRONMENT *IspEnvironment, char* pstr)
{
    if (stricmp(pstr, "-localecho") == 0)
    {
        IspEnvironment->LocalEcho = 1;
        DebugPrintf(3, "Local echo in terminal mode.\n");
        return TRUE;
    }

    if (stricmp(pstr, "-term") == 0)
    {
        IspEnvironment->TerminalAfterUpload = 1;
        DebugPrintf(3, "Invoke terminal after upload.\n");
        return TRUE;
    }

    if (stricmp(pstr, "-termonly") == 0)
    {
        IspEnvironment->TerminalOnly    = 1;
        IspEnvironment->ProgramChip    = 0;
        DebugPrintf(3, "Only provide terminal.\n");
        return TRUE;
    }

    return FALSE;
}

static BOOL escape_sequence;

void Terminal(ISP_ENVIRONMENT *IspEnvironment)
{
    if (IspEnvironment->TerminalAfterUpload || IspEnvironment->TerminalOnly)
    {
        int           ch = 0;
        char buffer[128];
        int           fdlogfile = -1;
        unsigned long realsize;

        // When logging is switched on, output terminal output to lpc21isp.log
        if (IspEnvironment->LogFile)
        {
            fdlogfile = open("lpc21isp.log", O_RDWR | O_BINARY | O_CREAT | O_TRUNC, 0777);
        }

        PrepareKeyboardTtySettings();

        DebugPrintf(1, "Terminal started (press Escape to abort)\n\n");
        fflush(stdout);

        escape_sequence = FALSE;

        while(1)
        {
            ReceiveComPort(IspEnvironment, buffer, sizeof(buffer) - 1, &realsize, 0,200);          // Check for received characters

			if (realsize){
				size_t wr=write(1, buffer, realsize);
				if(wr!=realsize)
					writeError(realsize,wr);
                fflush(stdout);
                if (IspEnvironment->LogFile)     // When logging is turned on, then copy output to logfile
                {
                    wr=write(fdlogfile, buffer, realsize);
                    if(wr!=realsize)
						writeError(realsize,wr);
                }
            }

            // check for keypress, and write any out the port.
            if (kbhit())
            {
                ch = getch();
                // if (escape_sequence) {

                // } else if (ch == 0x1b) {
                //     escape_sequence = true;
                // }
                // if (ch == 0x1b || escape_sequence)
                // {
                    
                //     break;
                // }

                buffer[0] = (unsigned char)ch;
                buffer[1] = 0;

                if (ch == 0x1b) {
                    if (kbhit()) {
                        char t = getch();
                        if (t == '[') { // We are in an escape character
                            if (kbhit()) {
                                t = getch();
                                switch (t) {
                                    case 'A': // Up arrow
                                        buffer[0] = '\033';
                                        buffer[1] = '[';
                                        buffer[2] = 'A';
                                        buffer[3] = 0;
                                        write(1, buffer, 3);
                                        break;
                                    case 'B': // Down arrow
                                        buffer[0] = '\033';
                                        buffer[1] = '[';
                                        buffer[2] = 'B';
                                        buffer[3] = 0;
                                        write(1, buffer, 3);
                                        break;
                                    case 'C': // Right arrow
                                        buffer[0] = '\033';
                                        buffer[1] = '[';
                                        buffer[2] = 'C';
                                        buffer[3] = 0;
                                        write(1, buffer, 3);
                                        break;
                                    case 'D': // Left arrow
                                        buffer[0] = '\033';
                                        buffer[1] = '[';
                                        buffer[2] = 'D';
                                        buffer[3] = 0;
                                        write(1, buffer, 3);
                                        break;
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }

                if (IspEnvironment->LocalEcho)
                {
					size_t wr=write(1, buffer, 1);
					if(wr!=1)
						writeError(1,wr);
                }

                SendComPort(IspEnvironment, buffer);
            }
        }

        DebugPrintf(1, "\n\nTerminal stopped\n\n");
        fflush(stdout);

        ResetKeyboardTtySettings();

        if (IspEnvironment->LogFile)
        {
            close(fdlogfile);
        }
    }
}
#endif
