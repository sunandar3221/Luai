#include "runtime.hpp"
#include "repl.hpp"
#include <iostream>
#include <string>
#include <vector>

static void printVersion() {
    std::cout << "Luai 1.0.0 (Bahasa Pemrograman Lua Dialek Bahasa Indonesia)\n";
    std::cout << "Hak Cipta (C) 2026 Engine Luai.\n";
    std::cout << "Berbasis Lua 5.4.7 (https://www.lua.org)\n";
}

static void printHelp() {
    printVersion();
    std::cout << "\nPenggunaan: luai [pilihan] [skrip [argumen...]]\n\n";
    std::cout << "Pilihan:\n";
    std::cout << "  -e kode        Mengeksekusi satu baris kode Luai\n";
    std::cout << "  -i             Masuk ke mode interaktif (REPL) setelah menjalankan skrip\n";
    std::cout << "  -v, --versi    Menampilkan informasi versi Luai\n";
    std::cout << "  -h, --bantuan  Menampilkan bantuan penggunaan ini\n\n";
    std::cout << "Mode Interaktif:\n";
    std::cout << "  Jalankan 'luai' tanpa argumen untuk masuk ke interactive shell (REPL).\n";
    std::cout << "  Ketik 'keluar' atau 'exit' untuk mengakhiri shell.\n";
}

int main(int argc, char* argv[]) {
    LuaiRuntime runtime;
    if (!runtime.init()) {
        std::cerr << runtime.getLastError() << "\n";
        return 1;
    }

    if (argc == 1) {
        Repl::run(runtime);
        return 0;
    }

    std::string firstArg = argv[1];

    if (firstArg == "-v" || firstArg == "--versi" || firstArg == "--version") {
        printVersion();
        return 0;
    }

    if (firstArg == "-h" || firstArg == "--bantuan" || firstArg == "--help") {
        printHelp();
        return 0;
    }

    if (firstArg == "-e") {
        if (argc < 3) {
            std::cerr << "Kesalahan: argumen untuk '-e' tidak diberikan\n";
            return 1;
        }
        std::string code = argv[2];
        if (!runtime.executeString(code, "baris_perintah")) {
            std::cerr << runtime.getLastError() << "\n";
            return 1;
        }
        return 0;
    }

    if (firstArg == "-i") {
        if (argc >= 3) {
            std::string filepath = argv[2];
            std::vector<std::string> scriptArgs;
            for (int i = 3; i < argc; i++) {
                scriptArgs.push_back(argv[i]);
            }
            if (!runtime.executeFile(filepath, scriptArgs)) {
                std::cerr << runtime.getLastError() << "\n";
            }
        }
        Repl::run(runtime);
        return 0;
    }

    std::string filepath = argv[1];
    std::vector<std::string> scriptArgs;
    for (int i = 2; i < argc; i++) {
        scriptArgs.push_back(argv[i]);
    }

    if (!runtime.executeFile(filepath, scriptArgs)) {
        std::cerr << runtime.getLastError() << "\n";
        return 1;
    }

    return 0;
}
