#include "runtime.hpp"
#include "repl.hpp"
#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static void printVersion() {
    std::cout << "Luai 1.0.1 (Bahasa Pemrograman Lua Dialek Bahasa Indonesia)\n";
    std::cout << "Hak Cipta (C) 2026 Engine Luai.\n";
    std::cout << "Engine   : " << luaiEngineName() << " - " << luaiEngineTagline() << "\n";
    std::cout << "Platform : " << luaiBuildPlatform() << "\n";
}

static void printHelp() {
    printVersion();
    std::cout << "\nPenggunaan: luai [pilihan] [skrip [argumen...]]\n\n";
    std::cout << "Pilihan Eksekusi:\n";
    std::cout << "  -e kode        Mengeksekusi satu baris kode Luai\n";
    std::cout << "  -i             Masuk ke mode interaktif (REPL) setelah menjalankan skrip\n";
    std::cout << "  -v, --versi    Menampilkan informasi versi Luai\n";
    std::cout << "  -h, --bantuan  Menampilkan bantuan penggunaan ini\n\n";
    std::cout << "Konversi Kode (Luai <-> Lua):\n";
    std::cout << "  --ke-lua berkas.luai [-o berkas.lua]    Mengonversi kode Luai ke Lua standar\n";
    std::cout << "  --ke-luai berkas.lua [-o berkas.luai]   Mengonversi kode Lua standar ke Luai\n";
    std::cout << "  --ke-lua -e \"kode\"                     Mengonversi sebaris kode Luai ke Lua\n";
    std::cout << "  --ke-luai -e \"kode\"                    Mengonversi sebaris kode Lua ke Luai\n\n";
    std::cout << "Untuk memulai REPL interaktif, jalankan 'luai' tanpa argumen.\n";
    std::cout << "  Ketik 'keluar' atau 'exit' untuk mengakhiri shell.\n";
}

static bool readFile(const std::string& path, std::string& content) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

static bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return file.good();
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string firstArg = argv[1];
        if (firstArg == "-v" || firstArg == "--versi" || firstArg == "--version") {
            printVersion();
            return 0;
        }
        if (firstArg == "-h" || firstArg == "--bantuan" || firstArg == "--help") {
            printHelp();
            return 0;
        }

        // Fitur Konversi: Luai -> Lua
        if (firstArg == "--ke-lua" || firstArg == "--to-lua" || firstArg == "--luai-ke-lua") {
            if (argc < 3) {
                std::cerr << "Kesalahan: berkas input atau opsi '-e' belum ditentukan.\n";
                std::cerr << "Contoh: luai --ke-lua skrip.luai [-o hasil.lua]\n";
                std::cerr << "        luai --ke-lua -e \"lokal x = 10; cetak(x)\"\n";
                return 1;
            }
            std::string secondArg = argv[2];
            if (secondArg == "-e") {
                if (argc < 4) {
                    std::cerr << "Kesalahan: kode untuk '-e' belum diberikan.\n";
                    return 1;
                }
                std::cout << Lexer::toLua(argv[3]) << "\n";
                return 0;
            }

            std::string inputPath = secondArg;
            std::string outputPath = "";
            for (int i = 3; i < argc; i++) {
                if (std::string(argv[i]) == "-o" && i + 1 < argc) {
                    outputPath = argv[i + 1];
                    break;
                }
            }

            std::string content;
            if (!readFile(inputPath, content)) {
                std::cerr << "Kesalahan: berkas '" << inputPath << "' tidak dapat dibaca atau tidak ditemukan.\n";
                return 1;
            }

            std::string converted = Lexer::toLua(content);
            if (!outputPath.empty()) {
                if (!writeFile(outputPath, converted)) {
                    std::cerr << "Kesalahan: gagal menulis hasil konversi ke berkas '" << outputPath << "'.\n";
                    return 1;
                }
                std::cout << "Berhasil mengonversi '" << inputPath << "' ke berkas Lua: '" << outputPath << "'\n";
            } else {
                std::cout << converted;
            }
            return 0;
        }

        // Fitur Konversi: Lua -> Luai
        if (firstArg == "--ke-luai" || firstArg == "--to-luai" || firstArg == "--lua-ke-luai") {
            if (argc < 3) {
                std::cerr << "Kesalahan: berkas input atau opsi '-e' belum ditentukan.\n";
                std::cerr << "Contoh: luai --ke-luai skrip.lua [-o hasil.luai]\n";
                std::cerr << "        luai --ke-luai -e \"local x = 10; print(x)\"\n";
                return 1;
            }
            std::string secondArg = argv[2];
            if (secondArg == "-e") {
                if (argc < 4) {
                    std::cerr << "Kesalahan: kode untuk '-e' belum diberikan.\n";
                    return 1;
                }
                std::cout << Lexer::toLuai(argv[3]) << "\n";
                return 0;
            }

            std::string inputPath = secondArg;
            std::string outputPath = "";
            for (int i = 3; i < argc; i++) {
                if (std::string(argv[i]) == "-o" && i + 1 < argc) {
                    outputPath = argv[i + 1];
                    break;
                }
            }

            std::string content;
            if (!readFile(inputPath, content)) {
                std::cerr << "Kesalahan: berkas '" << inputPath << "' tidak dapat dibaca atau tidak ditemukan.\n";
                return 1;
            }

            std::string converted = Lexer::toLuai(content);
            if (!outputPath.empty()) {
                if (!writeFile(outputPath, converted)) {
                    std::cerr << "Kesalahan: gagal menulis hasil konversi ke berkas '" << outputPath << "'.\n";
                    return 1;
                }
                std::cout << "Berhasil mengonversi '" << inputPath << "' ke berkas Luai: '" << outputPath << "'\n";
            } else {
                std::cout << converted;
            }
            return 0;
        }
    }

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
