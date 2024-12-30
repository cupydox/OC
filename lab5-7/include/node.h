#include <iostream>
#include "network.h"
#include <sstream>
#include <unordered_map>
#include "unistd.h"

class Node {
private:
    zmq::context_t context;
public:
    std::unordered_map<int, zmq::socket_t *> children;
    std::unordered_map<int, int> children_port;

    zmq::socket_t parent;
    int parent_port;
    int id;

    Node(int _id, int _parent_port = -1) : 
        parent(context, ZMQ_REP)
        , parent_port(_parent_port)
        , id(_id) 
    {
        if (_id != -1) {
            my_net::connect(&parent, _parent_port);
        }
    }

    std::string Ping_child(int _id) {
        if (_id == id) {
            return "Ok: 1";
        }
        int next_id = FindNextChild(_id);
        if (next_id == -1) {
            return "Ok: 0";
        }
        if (children.find(next_id) != children.end()) {
            std::string msg = "ping " + std::to_string(_id);
            my_net::send_message(children[next_id], msg);
            try {
                msg = my_net::receive(children[next_id]);
                return msg;
            } catch (int) {
                return "Ok: 0";
            }
        }
        return "Ok: 0";
    }

    std::string Create_child(int child_id, std::string program_path) {
        if (children.find(child_id) != children.end()) {
            return "Error: Already exists";
        }
        int next_id = FindNextChild(child_id);
        if (id == -1) {
            if (next_id == -1) {
                return ForkChild(child_id, program_path);
            } else {
                std::string ans = SendBST("create " + std::to_string(child_id), next_id);
                return ans;
            }
        } else {
            if (next_id == -1) {
                return ForkChild(child_id, program_path);
            } else {
                std::string ans = SendBST("create " + std::to_string(child_id), next_id);
                return ans;
            }
        }
    }

    std::string Pid() {
        return std::to_string(getpid());
    }

    std::string Send(std::string command, int _id) {
        if (_id == id) {
            return "Error: self request (not implemented direct?)";
        }
        int next_id = FindNextChild(_id);
        if (next_id == -1) {
            return "Error: not find"; 
        }
        return SendBST(command, next_id);
    }

    std::string Remove() {
        std::string ans;
        for (auto &child: children) {
            std::string ping_res = "Ok: 0";
            try {
                ping_res = Ping_child(child.first);
            } catch (...) {}
            if (ping_res == "Ok: 1") {
                my_net::send_message(child.second, "remove");
                try {
                    std::string msg = my_net::receive(child.second);
                    if (!msg.empty()) {
                        if (!ans.empty())
                            ans += " ";
                        ans += msg;
                    }
                } catch (...) {}
            }
            my_net::unbind(child.second, children_port[child.first]);
            child.second->close();
        }
        children.clear();
        children_port.clear();
        return ans;
    }

private:
    int FindNextChild(int _id) {
        if (children.empty()) return -1;
        if (id == -1) {
            if (children.size() == 1) {
                return children.begin()->first; 
            }
            return children.begin()->first;
        }

        if (_id < id) {
            int left_id = -1;
            for (auto &ch: children) {
                if (ch.first < id) {
                    left_id = ch.first;
                    break;
                }
            }
            return left_id;
        } else if (_id > id) {
            int right_id = -1;
            for (auto &ch: children) {
                if (ch.first > id) {
                    right_id = ch.first;
                    break;
                }
            }
            return right_id;
        }
        return -1;
    }

    std::string ForkChild(int child_id, const std::string &program_path) {
        std::string program_name = program_path.substr(program_path.find_last_of("/") + 1);
        children[child_id] = new zmq::socket_t(context, ZMQ_REQ);

        int new_port = my_net::bind(children[child_id], child_id);
        children_port[child_id] = new_port;

        int pid = fork();
        if (pid == 0) {
            execl(
                program_path.c_str(),      
                program_name.c_str(),       
                std::to_string(child_id).c_str(),
                std::to_string(new_port).c_str(),
                (char *) NULL
            );
        } else {
            std::string child_pid;
            try {
                children[child_id]->setsockopt(ZMQ_SNDTIMEO, 3000);
                my_net::send_message(children[child_id], "pid");
                child_pid = my_net::receive(children[child_id]);
            } catch (...) {
                child_pid = "Error: can't connect to child";
            }
            return "Ok: " + child_pid;
        }
        return "Error: fork error";
    }

    std::string SendBST(const std::string &msg, int next_id) {
        if (children.find(next_id) == children.end()) {
            return "Error: not find";
        }
        my_net::send_message(children[next_id], msg);
        try {
            std::string ans = my_net::receive(children[next_id]);
            return ans;
        } catch (...) {
            return "Error: not find";
        }
    }
};