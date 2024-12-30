#include "../include/node.h"
#include "../include/network.h"
#include <fstream>
#include <vector>
#include <signal.h>
#include <chrono>

int my_id = 0;

static bool timer_running = false;
static std::chrono::time_point<std::chrono::steady_clock> start_time;

int main(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    Node me(atoi(argv[1]), atoi(argv[2]));
    my_id = me.id;

    std::string prog_path = "./worker";
    while (true) {
        std::string message = my_net::receive(&(me.parent));
        std::istringstream request(message);
        std::string command;
        request >> command;

        if (command == "create") {
            int id_child;
            request >> id_child;
            std::string ans = me.Create_child(id_child, prog_path);
            my_net::send_message(&me.parent, ans);
        }
        else if (command == "pid") {
            std::string ans = me.Pid();
            my_net::send_message(&me.parent, ans);
        }
        else if (command == "ping") {
            int id_child;
            request >> id_child;
            std::string ans = me.Ping_child(id_child);
            my_net::send_message(&me.parent, ans);
        }
        else if (command == "send") {
            int id;
            request >> id;
            std::string str;
            getline(request, str);
            if (!str.empty()) {
                if (str[0] == ' ') {
                    str.erase(0,1);
                }
            }
            std::string ans = me.Send(str, id);
            my_net::send_message(&me.parent, ans);
        }
        else if (command == "exec") {
            std::string subcmd;
            request >> subcmd;
            std::string ans;
            if (subcmd == "start") {
                if (!timer_running) {
                    timer_running = true;
                    start_time = std::chrono::steady_clock::now();
                    ans = "Ok:" + std::to_string(me.id);
                } else {
                    ans = "Ok:" + std::to_string(me.id) + ":timer already running";
                }
            }
            else if (subcmd == "time") {
                if (timer_running) {
                    auto now = std::chrono::steady_clock::now();
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
                    ans = "Ok:" + std::to_string(me.id) + ":" + std::to_string(diff);
                } else {
                    ans = "Ok:" + std::to_string(me.id) + ":0";
                }
            }
            else if (subcmd == "stop") {
                if (timer_running) {
                    auto now = std::chrono::steady_clock::now();
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
                    timer_running = false;
                    ans = "Ok:" + std::to_string(me.id) + ":" + std::to_string(diff);
                } else {
                    ans = "Ok:" + std::to_string(me.id) + ":0";
                }
            }
            else {
                ans = "Error:" + std::to_string(me.id) + ": invalid exec command";
            }
            my_net::send_message(&me.parent, ans);
        }
        else if (command == "remove") {
            std::string ans = me.Remove();
            ans = std::to_string(me.id) + (ans.empty() ? "" : " " + ans);
            my_net::send_message(&me.parent, ans);

            my_net::disconnect(&me.parent, me.parent_port);
            me.parent.close();
            break;
        }
    }
    return 0;
}
