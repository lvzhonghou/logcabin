/* Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file
 * Changes the membership of a LogCabin cluster.
 */

#include <getopt.h>
#include <iostream>
#include <string>

#include <LogCabin/Client.h>

namespace {

using LogCabin::Client::Cluster;
using LogCabin::Client::Configuration;
using LogCabin::Client::ConfigurationResult;
using LogCabin::Client::Result;
using LogCabin::Client::Server;
using LogCabin::Client::Status;

/**
 * Parses argv for the main function.
 */
class OptionParser {
  public:
    OptionParser(int& argc, char**& argv)
        : argc(argc)
        , argv(argv)
        , cluster("logcabin:61023")
        , servers()
    {
        while (true) {
            static struct option longOptions[] = {
               {"cluster",  required_argument, NULL, 'c'},
               {"help",  no_argument, NULL, 'h'},
               {0, 0, 0, 0}
            };
            int c = getopt_long(argc, argv, "c:h", longOptions, NULL);

            // Detect the end of the options.
            if (c == -1)
                break;

            switch (c) {
                case 'c':
                    cluster = optarg;
                    break;
                case 'h':
                    usage();
                    exit(0);
                case '?':
                default:
                    // getopt_long already printed an error message.
                    usage();
                    exit(1);
            }
        }

        // Additional command line arguments are required.
        if (optind == argc) {
            usage();
            exit(1);
        }
        while (optind < argc) {
            servers.push_back(argv[optind]);
            ++optind;
        }
    }

    void usage() {
        std::cout << "Usage: " << argv[0] << " [options] <servers>"
                  << std::endl;
        std::cout << "Options: " << std::endl;
        std::cout << "  -c, --cluster <address> "
                  << "The network address of the LogCabin cluster "
                  << "(default: logcabin:61023)" << std::endl;
        std::cout << "  -h, --help              "
                  << "Print this usage information" << std::endl;
    }

    int& argc;
    char**& argv;
    std::string cluster;
    std::vector<std::string> servers;
};

void
printConfiguration(const std::pair<uint64_t, Configuration>& configuration)
{
    std::cout << "Configuration " << configuration.first << ":" << std::endl;
    for (auto it = configuration.second.begin();
         it != configuration.second.end();
         ++it) {
        std::cout << "- " << it->serverId << ": " << it->addresses
                  << std::endl;
    }
    std::cout << std::endl;
}


} // anonymous namespace

int
main(int argc, char** argv)
{
    OptionParser options(argc, argv);
    Cluster cluster(options.cluster);

    std::pair<uint64_t, Configuration> configuration =
        cluster.getConfiguration();
    uint64_t id = configuration.first;
    std::cout << "Current configuration:" << std::endl;
    printConfiguration(configuration);

    std::cout << "Attempting to change cluster membership to the following:"
              << std::endl;
    Configuration servers;
    for (auto it = options.servers.begin();
         it != options.servers.end();
         ++it) {
        Server info;
        Result result = cluster.getServerInfo(*it,
                                              /* timeout = 2s */ 2000000000UL,
                                              info);
        switch (result.status) {
            case Status::OK:
                std::cout << info.serverId << ": "
                          << info.addresses
                          << " (given as " << *it << ")"
                          << std::endl;
                servers.emplace_back(info.serverId, info.addresses);
                break;
            case Status::TIMEOUT:
                std::cout << "Could not fetch server info from "
                          << *it << " (" << result.error << "). Aborting."
                          << std::endl;
                return 1;
            default:
                std::cout << "Unknown error from "
                          << *it << " (" << result.error << "). Aborting."
                          << std::endl;
                return 1;
        }
    }
    std::cout << std::endl;

    ConfigurationResult result = cluster.setConfiguration(id, servers);
    std::cout << "Membership change result: ";
    if (result.status == ConfigurationResult::OK) {
        std::cout << "OK" << std::endl;
    } else if (result.status == ConfigurationResult::CHANGED) {
        std::cout << "CHANGED (" << result.error << ")" << std::endl;
    } else if (result.status == ConfigurationResult::BAD) {
        std::cout << "BAD SERVERS (" << result.error << "):" << std::endl;
        for (auto it = result.badServers.begin();
             it != result.badServers.end();
             ++it) {
            std::cout << "- " << it->serverId << ": " << it->addresses
                      << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "Current configuration:" << std::endl;
    printConfiguration(cluster.getConfiguration());

    if (result.status == ConfigurationResult::OK)
        return 0;
    else
        return 1;
}
