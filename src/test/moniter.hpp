#include <poll.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <test.hpp>
#include <vector>

class Monitor {
   public:
    Monitor(const std::string& make_dir, const std::vector<std::string>& s)
        : make_dir{make_dir}, _filepaths{s}, _fd{-1} {
        wd.resize(s.size());
        for (int& x : wd) {
            x = -1;
        }
    }

    void runLoop() {
        createNotifyFd();
        // std::cout << "created notify_fd" << std::endl;
        addFilesToWatch();
        // std::cout << "added files to notify_fd" << std::endl;

        nfds_t nfds = 1;
        struct pollfd fds[1];

        fds[0].fd = _fd;
        fds[1].events = POLLIN;

        std::cout << "Watching for files!" << std::endl;
        std::cout << "Loading library...\n" << std::endl;
        TestModule::LoadLibrary();
        TestModule::Run();
        // std::cout << "\nRan the library!" << std::endl;
        int count = 0;
        while (1) {
            int poll_num = poll(fds, nfds, -1);
            // std::cout << "got an event!" << (count++) << std::endl;
            if (poll_num == -1) {
                if (errno == EINTR) {
                    continue;
                }
                throw std::runtime_error(std::string("Error in poll() : ") +
                                         std::string(strerror(errno)));
            }

            if (poll_num > 0) {
                if (fds[0].revents & POLLIN) {
                    // reload library!
                    // std::cout << "Reloading!\n" << std::endl;
                    handleModifyEvent();
                }
            }
        }
    }

    ~Monitor() { close(_fd); }

   private:
    void handleModifyEvent() {
        char buf[4096]
            __attribute__((aligned(__alignof__(struct inotify_event))));

        const struct inotify_event* event;
        ssize_t len;

        for (;;) {
            // std::cout << "Reading!\n" << std::endl;
            len = read(_fd, buf, sizeof(buf));
            if (len == -1 && errno != EAGAIN) {
                throw std::runtime_error(std::string("read() : ") +
                                         std::string(strerror(errno)));
            }
            // std::cout << "len : " << len << std::endl;
            if (len <= 0) {
                break;
            }

            for (char* ptr = buf; ptr < buf + len;
                 ptr += sizeof(struct inotify_event) + event->len) {
                event = reinterpret_cast<const struct inotify_event*>(ptr);
                if (event->mask & IN_IGNORED || event->mask & IN_CLOSE_WRITE) {
                    // recompile using make/premake5 and reload
                    // the library

                    if (event->mask & IN_MODIFY) {
                        // std::cout << "MODIFY" << std::endl;
                    }
                    if (event->mask & IN_IGNORED) {
                        // std::cout << "IN_IGNORED" << std::endl;
                    }
                    if (event->mask & IN_CLOSE_WRITE) {
                        // std::cout << "IN_CLOSE_WRITE" << std::endl;
                    }

                    std::cout << "Detected change " << event->name
                              << "\nReloading libray..\n";
                    if (chdir(make_dir.c_str()) != 0) {
                        throw std::runtime_error(std::string("chdir :") +
                                                 std::string(strerror(errno)));
                    }

                    int result = std::system("make > /dev/null 2>&1");

                    if (result != 0) {
                        std::cerr << "Error executing make command : "
                                  << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Make executed succesfully, reloading "
                                     "library!\n\n";
                        TestModule::ReloadLibrary();
                        TestModule::Run();
                    }
                }

                for (size_t i = 1; i < wd.size(); i++) {
                    if (wd[i] == event->wd) {
                        std::cout << _filepaths[i] << std::endl;
                        break;
                    }
                }

                if (event->len) std::cout << event->name;
                // if (event->mask & IN_ISDIR)
                //     std::cout << " [directory]" << std::endl;
                // else
                //     std::cout << " [file]" << std::endl;
            }
        }
    }
    void createNotifyFd() {
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd == -1) {
            throw std::runtime_error("Error initializing inotify: " +
                                     std::string(strerror(errno)));
        }
        _fd = fd;
        std::cout << "created notify_fd" << std::endl;
    }

    void addFilesToWatch() {
        for (int j = 0; j < wd.size(); j++) {
            std::string file_path = make_dir + _filepaths[j];
            wd[j] = inotify_add_watch(_fd, file_path.c_str(),
                                      IN_MODIFY | IN_IGNORED | IN_CLOSE_WRITE);
            if (wd[j] == -1) {
                throw std::runtime_error(
                    "Error adding " + file_path +
                    " to watcher inotify: " + std::string(strerror(errno)));
            }
        }
        std::cout << "added files to notify_fd" << std::endl;
    }

    std::vector<std::string>
        _filepaths;       // file paths (for now strictly filepath)
    int _fd;              // file descriptor for inotify
    std::vector<int> wd;  // watch descriptors
    std::string make_dir;
};