/**
 * Name:        wxPTB.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-07-09
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef WXPTB_H
#define WXPTB_H

#define PTB_APP_NAME                "wxPTB"
#define PTB_VERSION_MAJOR           0
#define PTB_VERSION_MINOR           3
#define PTB_VERSION_RELEASE         0
#define PTB_VERSION_EXTENSION       "alpha"
#define PTB_AUTHOR                  "Christian Buhtz"

#define PTB_URL_TIMESTAMP           "http://publictimestamp.org/rest/v1.0/publictimestamp-rest-v1.0.pl?pt=getlatestptb"

#define PTB_CONFIG_DIR              ".wxptb"

#define PTB_OUT_SUFFIX              ".sig"
#define PTB_HASH_ALL                "all"

#define PTB_ERR_URL                 "Can not create the URL!"
#define PTB_ERR_STREAM              "Can not create the input stream!\nMaybe you are offline?\nPlease check your internet connection!"
#define PTB_ERR_XMLDOC              "Can not parse the xml file!"

#endif
