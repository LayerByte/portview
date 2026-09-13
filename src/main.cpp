#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

constexpr const char* PROJECT_NAME = "Portview";
constexpr const char* PROJECT_FOCUS = "Display local listening ports and connection information.";

std::vector<unsigned char> read_bytes(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Could not open file.");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

double entropy(const std::vector<unsigned char>& data) {
    if (data.empty()) return 0.0;
    std::array<size_t, 256> counts{};
    for (auto byte : data) counts[byte]++;
    double result = 0.0;
    for (auto count : counts) {
        if (count == 0) continue;
        double p = static_cast<double>(count) / static_cast<double>(data.size());
        result -= p * std::log2(p);
    }
    return result;
}

uint32_t rotate_right(uint32_t value, uint32_t bits) {
    return (value >> bits) | (value << (32 - bits));
}

std::string sha256(const std::vector<unsigned char>& data) {
    static constexpr std::array<uint32_t, 64> k = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::array<uint32_t, 8> h = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };
    std::vector<unsigned char> message = data;
    uint64_t bit_length = static_cast<uint64_t>(message.size()) * 8;
    message.push_back(0x80);
    while ((message.size() % 64) != 56) message.push_back(0);
    for (int i = 7; i >= 0; --i) message.push_back(static_cast<unsigned char>((bit_length >> (i * 8)) & 0xff));

    for (size_t offset = 0; offset < message.size(); offset += 64) {
        std::array<uint32_t, 64> w{};
        for (size_t i = 0; i < 16; ++i) {
            size_t j = offset + i * 4;
            w[i] = (static_cast<uint32_t>(message[j]) << 24) |
                   (static_cast<uint32_t>(message[j + 1]) << 16) |
                   (static_cast<uint32_t>(message[j + 2]) << 8) |
                   static_cast<uint32_t>(message[j + 3]);
        }
        for (size_t i = 16; i < 64; ++i) {
            uint32_t s0 = rotate_right(w[i - 15], 7) ^ rotate_right(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotate_right(w[i - 2], 17) ^ rotate_right(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (size_t i = 0; i < 64; ++i) {
            uint32_t s1 = rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = hh + s1 + ch + k[i] + w[i];
            uint32_t s0 = rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = s0 + maj;
            hh = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }
    std::ostringstream out;
    for (auto part : h) out << std::hex << std::setw(8) << std::setfill('0') << part;
    return out.str();
}

void inspect_file(const fs::path& path) {
    if (!fs::is_regular_file(path)) throw std::runtime_error("Expected a readable local file.");
    auto data = read_bytes(path);
    std::cout << "Project: " << PROJECT_NAME << "\n";
    std::cout << "Focus: " << PROJECT_FOCUS << "\n";
    std::cout << "Path: " << fs::absolute(path).string() << "\n";
    std::cout << "Size: " << data.size() << " bytes\n";
    std::cout << "Entropy: " << std::fixed << std::setprecision(4) << entropy(data) << "\n";
    std::cout << "SHA-256: " << sha256(data) << "\n";
    std::cout << "Header: ";
    for (size_t i = 0; i < std::min<size_t>(16, data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

void extract_strings(const fs::path& path) {
    auto data = read_bytes(path);
    std::string current;
    for (auto byte : data) {
        if (std::isprint(byte)) {
            current.push_back(static_cast<char>(byte));
        } else {
            if (current.size() >= 4) std::cout << current << "\n";
            current.clear();
        }
    }
    if (current.size() >= 4) std::cout << current << "\n";
}

uint32_t parse_ipv4(const std::string& value) {
    std::stringstream stream(value);
    std::string part;
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(stream, part, '.')) throw std::runtime_error("Invalid IPv4 address.");
        int octet = std::stoi(part);
        if (octet < 0 || octet > 255) throw std::runtime_error("Invalid IPv4 octet.");
        result = (result << 8) | static_cast<uint32_t>(octet);
    }
    return result;
}

std::string format_ipv4(uint32_t value) {
    std::ostringstream out;
    out << ((value >> 24) & 255) << "." << ((value >> 16) & 255) << "." << ((value >> 8) & 255) << "." << (value & 255);
    return out.str();
}

void cidr_report(const std::string& cidr) {
    auto slash = cidr.find('/');
    if (slash == std::string::npos) throw std::runtime_error("CIDR must look like 192.168.1.0/24.");
    auto ip = parse_ipv4(cidr.substr(0, slash));
    int prefix = std::stoi(cidr.substr(slash + 1));
    if (prefix < 0 || prefix > 32) throw std::runtime_error("CIDR prefix must be 0-32.");
    uint32_t mask = prefix == 0 ? 0 : (0xffffffffu << (32 - prefix));
    uint32_t network = ip & mask;
    uint32_t broadcast = network | ~mask;
    std::cout << "Network: " << format_ipv4(network) << "\n";
    std::cout << "Broadcast: " << format_ipv4(broadcast) << "\n";
    std::cout << "Usable range: " << format_ipv4(network + 1) << " - " << format_ipv4(broadcast - 1) << "\n";
}

int main(int argc, char** argv) {
    try {
        std::cout << PROJECT_NAME << " - " << PROJECT_FOCUS << "\n";
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " <file-path|cidr> [--strings]\n";
            return 0;
        }
        std::string input = argv[1];
        if (input.find('/') != std::string::npos && input.find('.') != std::string::npos && !fs::exists(input)) {
            cidr_report(input);
        } else if (argc >= 3 && std::string(argv[2]) == "--strings") {
            extract_strings(input);
        } else {
            inspect_file(input);
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
