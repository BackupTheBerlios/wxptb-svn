/**
 * Name:        PTBConfig.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-07-19
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


#ifndef PTBCONFIG_H
#define PTBCONFIG_H

///
class PTBConfig
{
    private:
        /** Maximum size of the log-file in KiloBytes.
            Set it to '0' if the maximum size does not matter. */
        long                    lMaxSize_;

        /** How many times should the download of the PTB be retried
            if it failes. */
        long                    lLoadRetry_;

        /** Time to wait in seconds till the next retry of PTB download
            if it was failed. */
        long                    lLoadDelay_;

    public:
        ///
        PTBConfig ();
        /// virtual dtor
        virtual ~PTBConfig ();

        ///
        void Init ();

        /// Read the data from the config file.
        void ReadConfig ();
        /// Save the data to the config file.
        bool SaveConfig ();

        ///
        void SetMaxLogFileSizeInKB (long lSize);
        ///
        long GetMaxLogFileSizeInKB () const;
        ///
        void SetLoadRetry (long lRetry);
        ///
        long GetLoadRetry () const;
        ///
        void SetLoadDelayInSeconds (long lDelay);
        ///
        long GetLoadDelayInSeconds () const;
};

#endif
