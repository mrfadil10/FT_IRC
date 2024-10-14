#include "../includes/Server.hpp"

int bot_info(int fd_bot, const std::string &cmd) {
    write(fd_bot,cmd.c_str(),cmd.length());
    return 0;
}

int is_privmsg(int fd_bot, std::string str, std::string nick) {
        std::string cmd;
        size_t indx = str.find(" "), i = 0;
        while(indx != str.npos) {
            i++;
            if(i == 3) {
                cmd = str.substr(indx + 1);
                if (cmd.at(0) == ':')
                    cmd = cmd.substr(1);
                if (cmd.at(cmd.length() - 1) == '\n')
                    cmd.erase(cmd.begin() + cmd.size() - 1);
                if (cmd.at(cmd.length() - 1) == '\r')
                    cmd.erase(cmd.begin() + cmd.size() - 1);
                break;
            }
            indx = str.find(" ", indx + 1);
        } if (cmd == "joke") {
            std::string line;
            int rand_val = std::rand() % 20 + 1, i = 0;
            std::ifstream f_joke("./Bot/Jokes.txt");
            if(f_joke.fail()) {
                return 1;
            } while (getline(f_joke, line)) {
                if(i == rand_val) {
                    std::string str1 = "PRIVMSG " + nick + " :" + line + " \r\n";
                    send(fd_bot, str1.c_str(),str1.length(),0);
                    f_joke.close();
                    break;
                }
                i++;
            }
        }
        else {
            std::string str1 = "PRIVMSG " + nick + " : Just \" joke \" ,Try Again..\r\n";
            send(fd_bot, str1.c_str(),str1.length(),0);
        }
        return 0;
}

int main(int ac, char *av[]) {
    if (ac != 5) {
        std::cout << "U must folow this steps : address -> port -> nickname -> password .\nTry Again.. :/" << std::endl;
        return EXIT_FAILURE;
    }
    int fd_bot = 0;
    std::string pass = av[4];
    std::string nick = av[3];
    struct sockaddr_in __bot;
    std::cout << "The Bot will be Installed on the Server with Nick_Name : \" " << nick << " \", Enjoy ;)" << std::endl;
    fd_bot = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_bot < 0) {
        std::perror("socket");
        exit(EXIT_FAILURE);
    }
    std::memset(&__bot, 0, sizeof(__bot));
    __bot.sin_family = AF_INET;
    __bot.sin_port = htons(std::atoi(av[2]));
    __bot.sin_addr.s_addr = inet_addr(av[1]);
	if (connect(fd_bot, (struct sockaddr*)&__bot, sizeof(__bot)) != 0) {
        close(fd_bot);
        std::perror("connect");
        return EXIT_FAILURE;
    }
    bot_info(fd_bot, "PASS " + pass + "\r\n");
    usleep(700);
    bot_info(fd_bot, "USER boot bot bot boot\r\n");
    usleep(700);
    bot_info(fd_bot, "NICK "+nick+"\r\n");
    char buffer[512];
    std::string str, rr;
    while (1) {
        std::memset(buffer, 0, 512);
        ssize_t readd = read(fd_bot, buffer, 511);
        if (readd <= 0)
            break;
        buffer[readd] = '\0';
        str = buffer;
        if (str.empty())
            continue;
        rr = str.substr(str.find(" ") + 1, 3);
        if (rr == "464" || rr == "461") {
            std::cout << "U must put the correct Password ,Try Again.. :/" << std::endl;
            close(fd_bot);
            return 1;
        }
        if (rr == "433" || rr == "432" || rr == "431") {
            std::cout << "This nickname is used or Error in NickName, Try Again.. :/" << std::endl;
            close(fd_bot);
            return 1;
        }
        if (str.substr(0, 4) == "PING") {
            std::string num_pong = "PONG " + str.substr(6);
            send(fd_bot, num_pong.c_str(),num_pong.length(),0);
        }
        std::string test;
        test = str.substr(str.find(" ") + 1, 7);
        if (test == "PRIVMSG")
        {
            std::string nick = str.substr(1, str.find("!") - 1);
            if(is_privmsg(fd_bot, str, nick)) {
                std::string str1 = "PRIVMSG " + nick + " :no jokes in database, Try the command again..\r\n";
                send(fd_bot, str1.c_str(),str1.length(),0);
                continue;
            }
            nick.clear();
        }
    }
    close(fd_bot);
    return EXIT_SUCCESS;
}
