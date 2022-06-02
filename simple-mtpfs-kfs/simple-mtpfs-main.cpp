/* ***** BEGIN LICENSE BLOCK *****
*   Copyright (C) 2012-2016, Peter Hatina <phatina@gmail.com>
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License as
*   published by the Free Software Foundation; either version 2 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
* ***** END LICENSE BLOCK ***** */

#include <libgen.h>
#include <iostream>
#include "simple-mtpfs-kfs.h"
#include "simple-mtpfs-util.h"
#include <pthread.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>

#define NFSSVC_NFSD     0x004


pthread_mutex_t control_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t control_cv = PTHREAD_COND_INITIALIZER;
int in_fs = 0;
static SMTPFileSystem *filesystem = new SMTPFileSystem;

static void init(void){
    pthread_mutex_init(&control_mtx, PTHREAD_MUTEX_NORMAL);
    pthread_cond_init(&control_cv, nullptr);
}

int main(int argc, char **argv)
{

    if (!filesystem->parseOptions(argc, argv)) {
        std::cout << "Wrong usage! See `" << smtpfs_basename(argv[0])
            << " -h' for details\n";
        return 1;
    }

    if (filesystem->isHelp()) {
        filesystem->printHelp(argv[0]);
        return 0;
    }

    if (filesystem->isVersion()) {
        filesystem->printVersion();
        return 0;
    }

    if (filesystem->isListDevices())
        return !filesystem->listDevices();

    SMTPcontext_t ctx = {filesystem};
    kfs_set_thread_begin_callback((void (*)())init);
    if(filesystem->mount(&ctx)){
//        CFRunLoopRun();
//        dispatch_main();
//        pthread_create(&svc_thrid, NULL, (void *(*)(void *))nfsd_server_thread, NULL);
//        if (nfssvc(NFSSVC_NFSD, NULL) < 0)
//            fprintf(stderr, "nfssvc: %s (%d)", strerror(errno), errno);
//        fprintf(stdout, "nfsd thread exiting.");

        for (;;){
            if(in_fs){
                // if the fs code is executing continue looping.
                pthread_yield_np();
            }
            // else wait
            pthread_cond_wait(&control_cv, &control_mtx);
        }
    } else return -1;

    return 0;
}
