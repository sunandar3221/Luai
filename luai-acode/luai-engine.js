(function (root, factory) {
  if (typeof define === "function" && define.amd) {
    define([], factory);
  } else if (typeof module === "object" && module.exports) {
    module.exports = factory();
  } else {
    root.LuaiEngine = factory();
  }
})(typeof self !== "undefined" ? self : this, function () {
  function getFengari() {
    if (typeof fengari !== "undefined") {
      return fengari;
    }
    if (typeof window !== "undefined" && window.fengari) {
      return window.fengari;
    }
    if (typeof globalThis !== "undefined" && globalThis.fengari) {
      return globalThis.fengari;
    }
    if (typeof require === "function") {
      try {
        return require("./fengari-web.js");
      } catch (e) {
        try {
          return require("fengari-web");
        } catch (e2) {}
      }
    }
    return null;
  }

  const keywordMap = {
    dan: "and",
    berhenti: "break",
    lakukan: "do",
    selain_itu: "else",
    atau_jika: "elseif",
    selesai: "end",
    salah: "false",
    untuk: "for",
    fungsi: "function",
    lompat_ke: "goto",
    jika: "if",
    dalam: "in",
    lokal: "local",
    kosong: "nil",
    nihil: "nil",
    bukan: "not",
    atau: "or",
    ulang: "repeat",
    kembalikan: "return",
    maka: "then",
    benar: "true",
    sampai: "until",
    selama: "while"
  };

  function transpile(source) {
    let result = "";
    let i = 0;
    const n = source.length;

    if (source.charCodeAt(0) === 0xFEFF) {
      i = 1;
    }

    while (i < n) {
      if (i === 0 && source[i] === "#") {
        while (i < n && source[i] !== "\n") {
          result += source[i];
          i++;
        }
        continue;
      }

      if (i + 1 < n && source[i] === "-" && source[i + 1] === "-") {
        let j = i + 2;
        if (j < n && source[j] === "[") {
          let k = j + 1;
          let eq = 0;
          while (k < n && source[k] === "=") {
            eq++;
            k++;
          }
          if (k < n && source[k] === "[") {
            const closeTag = "]" + "=".repeat(eq) + "]";
            const closePos = source.indexOf(closeTag, k + 1);
            if (closePos !== -1) {
              const endPos = closePos + closeTag.length;
              result += source.substring(i, endPos);
              i = endPos;
            } else {
              result += source.substring(i);
              i = n;
            }
            continue;
          }
        }
        while (i < n && source[i] !== "\n") {
          result += source[i];
          i++;
        }
        continue;
      }

      if (source[i] === "\"" || source[i] === "'") {
        const quote = source[i];
        result += quote;
        i++;
        while (i < n) {
          const sc = source[i];
          if (sc === "\\") {
            result += sc;
            i++;
            if (i < n) {
              result += source[i];
              i++;
            }
          } else if (sc === quote) {
            result += sc;
            i++;
            break;
          } else {
            result += sc;
            i++;
          }
        }
        continue;
      }

      if (source[i] === "[") {
        let j = i + 1;
        let eq = 0;
        while (j < n && source[j] === "=") {
          eq++;
          j++;
        }
        if (j < n && source[j] === "[") {
          const closeTag = "]" + "=".repeat(eq) + "]";
          const closePos = source.indexOf(closeTag, j + 1);
          if (closePos !== -1) {
            const endPos = closePos + closeTag.length;
            result += source.substring(i, endPos);
            i = endPos;
          } else {
            result += source.substring(i);
            i = n;
          }
          continue;
        }
      }

      if (/[a-zA-Z_]/.test(source[i])) {
        const start = i;
        while (i < n && /[a-zA-Z0-9_]/.test(source[i])) {
          i++;
        }
        const ident = source.substring(start, i);
        if (Object.prototype.hasOwnProperty.call(keywordMap, ident)) {
          result += keywordMap[ident];
        } else {
          result += ident;
        }
        continue;
      }

      result += source[i];
      i++;
    }

    return result;
  }

  function initRuntime(onPrint, onRead) {
    const f = getFengari();
    if (!f) {
      throw new Error("Engine Lua Fengari tidak ditemukan.");
    }

    const lua = f.lua;
    const lauxlib = f.lauxlib;
    const lualib = f.lualib;
    const to_luastring = f.to_luastring;
    const to_jsstring = f.to_jsstring;

    const L = lauxlib.luaL_newstate();
    lualib.luaL_openlibs(L);

    lua.lua_pushjsfunction(L, function (L) {
      const top = lua.lua_gettop(L);
      const parts = [];
      for (let i = 1; i <= top; i++) {
        if (lua.lua_isboolean(L, i)) {
          parts.push(lua.lua_toboolean(L, i) ? "benar" : "salah");
        } else if (lua.lua_isnil(L, i)) {
          parts.push("nihil");
        } else {
          const s = lauxlib.luaL_tolstring(L, i);
          parts.push(to_jsstring(s));
          lua.lua_pop(L, 1);
        }
      }
      if (typeof onPrint === "function") {
        onPrint(parts.join("\t"));
      }
      return 0;
    });
    lua.lua_setglobal(L, to_luastring("cetak"));

    lua.lua_pushjsfunction(L, function (L) {
      lauxlib.luaL_checkany(L, 1);
      const t = lua.lua_type(L, 1);
      let res = "tidak_dikenal";
      switch (t) {
        case lua.LUA_TNIL:
          res = "nihil";
          break;
        case lua.LUA_TNUMBER:
          res = "angka";
          break;
        case lua.LUA_TBOOLEAN:
          res = "boolean";
          break;
        case lua.LUA_TSTRING:
          res = "teks";
          break;
        case lua.LUA_TTABLE:
          res = "tabel";
          break;
        case lua.LUA_TFUNCTION:
          res = "fungsi";
          break;
        case lua.LUA_TUSERDATA:
        case lua.LUA_TLIGHTUSERDATA:
          res = "userdata";
          break;
        case lua.LUA_TTHREAD:
          res = "korutin";
          break;
      }
      lua.lua_pushstring(L, to_luastring(res));
      return 1;
    });
    lua.lua_setglobal(L, to_luastring("tipe"));

    lua.lua_pushjsfunction(L, function (L) {
      const base = lauxlib.luaL_optinteger(L, 2, 10);
      if (base === 10) {
        if (lua.lua_isnumber(L, 1)) {
          lua.lua_pushnumber(L, lua.lua_tonumber(L, 1));
          return 1;
        }
        if (lua.lua_isstring(L, 1)) {
          const str = to_jsstring(lua.lua_tostring(L, 1));
          const num = Number(str);
          if (!isNaN(num)) {
            lua.lua_pushnumber(L, num);
            return 1;
          }
        }
      } else {
        const str = to_jsstring(lauxlib.luaL_checkstring(L, 1));
        const num = parseInt(str, base);
        if (!isNaN(num)) {
          lua.lua_pushinteger(L, num);
          return 1;
        }
      }
      lua.lua_pushnil(L);
      return 1;
    });
    lua.lua_setglobal(L, to_luastring("ke_angka"));

    lua.lua_pushjsfunction(L, function (L) {
      lauxlib.luaL_checkany(L, 1);
      if (lua.lua_isboolean(L, 1)) {
        lua.lua_pushstring(L, to_luastring(lua.lua_toboolean(L, 1) ? "benar" : "salah"));
        return 1;
      }
      if (lua.lua_isnil(L, 1)) {
        lua.lua_pushstring(L, to_luastring("nihil"));
        return 1;
      }
      lauxlib.luaL_tolstring(L, 1);
      return 1;
    });
    lua.lua_setglobal(L, to_luastring("ke_teks"));

    lua.lua_pushjsfunction(L, function (L) {
      lauxlib.luaL_checkany(L, 1);
      lua.lua_getglobal(L, to_luastring("pairs"));
      lua.lua_pushvalue(L, 1);
      lua.lua_call(L, 1, 3);
      return 3;
    });
    lua.lua_setglobal(L, to_luastring("pasangan"));

    lua.lua_pushjsfunction(L, function (L) {
      lauxlib.luaL_checkany(L, 1);
      lua.lua_getglobal(L, to_luastring("ipairs"));
      lua.lua_pushvalue(L, 1);
      lua.lua_call(L, 1, 3);
      return 3;
    });
    lua.lua_setglobal(L, to_luastring("i_pasangan"));

    lua.lua_pushjsfunction(L, function (L) {
      const top = lua.lua_gettop(L);
      let promptText = "";
      if (top >= 1 && lua.lua_isstring(L, 1)) {
        const s = to_jsstring(lua.lua_tostring(L, 1));
        if (s && s[0] !== "*") {
          promptText = s;
        }
      }
      let line = "";
      if (typeof onRead === "function") {
        line = onRead(promptText) || "";
      } else if (typeof prompt === "function") {
        line = prompt(promptText || "Luai Input:") || "";
      }
      lua.lua_pushstring(L, to_luastring(line));
      return 1;
    });
    lua.lua_setglobal(L, to_luastring("masukan"));

    lua.lua_pushjsfunction(L, function (L) {
      lua.lua_getglobal(L, to_luastring("table"));
      lua.lua_getfield(L, -1, to_luastring("unpack"));
      const top = lua.lua_gettop(L) - 2;
      for (let i = 1; i <= top; i++) {
        lua.lua_pushvalue(L, i);
      }
      lua.lua_call(L, top, lua.LUA_MULTRET);
      return lua.lua_gettop(L) - (top + 2);
    });
    lua.lua_setglobal(L, to_luastring("lepas"));

    const aliasList = [
      ["assert", "tegaskan"],
      ["error", "kesalahan"],
      ["pcall", "pcall_aman"],
      ["pcall", "panggil_aman"],
      ["xpcall", "xpcall_aman"],
      ["setmetatable", "set_metatabel"],
      ["getmetatable", "ambil_metatabel"],
      ["collectgarbage", "koleksi_sampah"],
      ["select", "pilih"],
      ["load", "muat"],
      ["loadfile", "muat_file"],
      ["dofile", "eksekusi_file"],
      ["require", "butuh"],
      ["require", "perlu"]
    ];

    for (let i = 0; i < aliasList.length; i++) {
      lua.lua_getglobal(L, to_luastring(aliasList[i][0]));
      lua.lua_setglobal(L, to_luastring(aliasList[i][1]));
    }

    const copyModule = function (srcName, dstName, fnAliases) {
      lua.lua_getglobal(L, to_luastring(srcName));
      if (lua.lua_istable(L, -1)) {
        lua.lua_newtable(L);
        lua.lua_pushnil(L);
        while (lua.lua_next(L, -3) !== 0) {
          lua.lua_pushvalue(L, -2);
          lua.lua_insert(L, -2);
          lua.lua_settable(L, -4);
        }
        for (let j = 0; j < fnAliases.length; j++) {
          lua.lua_getfield(L, -2, to_luastring(fnAliases[j][1]));
          if (!lua.lua_isnil(L, -1)) {
            lua.lua_setfield(L, -2, to_luastring(fnAliases[j][0]));
          } else {
            lua.lua_pop(L, 1);
          }
        }
        lua.lua_setglobal(L, to_luastring(dstName));
      }
      lua.lua_pop(L, 1);
    };

    copyModule("math", "matematika", [
      ["akar", "sqrt"],
      ["mutlak", "abs"],
      ["lantai", "floor"],
      ["atap", "ceil"],
      ["acak", "random"],
      ["benih_acak", "randomseed"],
      ["minimum", "min"],
      ["maksimum", "max"],
      ["derajat", "deg"],
      ["radian", "rad"]
    ]);

    copyModule("table", "tabel", [
      ["sisip", "insert"],
      ["hapus", "remove"],
      ["urut", "sort"],
      ["gabung", "concat"],
      ["pindah", "move"],
      ["bungkus", "pack"],
      ["lepas", "unpack"]
    ]);

    copyModule("string", "teks", [
      ["panjang", "len"],
      ["huruf_besar", "upper"],
      ["huruf_kecil", "lower"],
      ["potong", "sub"],
      ["cari", "find"],
      ["ganti", "gsub"],
      ["cocok", "match"],
      ["format", "format"],
      ["ulang", "rep"],
      ["balik", "reverse"],
      ["karakter", "char"],
      ["byte", "byte"]
    ]);

    lua.lua_getglobal(L, to_luastring("string"));
    if (lua.lua_istable(L, -1)) {
      const strAliases = [
        ["panjang", "len"],
        ["huruf_besar", "upper"],
        ["huruf_kecil", "lower"],
        ["potong", "sub"],
        ["cari", "find"],
        ["ganti", "gsub"],
        ["cocok", "match"],
        ["format", "format"],
        ["ulang", "rep"],
        ["balik", "reverse"],
        ["karakter", "char"],
        ["byte", "byte"]
      ];
      for (let j = 0; j < strAliases.length; j++) {
        lua.lua_getfield(L, -1, to_luastring(strAliases[j][1]));
        if (!lua.lua_isnil(L, -1)) {
          lua.lua_setfield(L, -2, to_luastring(strAliases[j][0]));
        } else {
          lua.lua_pop(L, 1);
        }
      }
    }
    lua.lua_pop(L, 1);

    copyModule("os", "sistem", [
      ["waktu", "time"],
      ["jam", "clock"],
      ["tanggal", "date"],
      ["keluar", "exit"],
      ["jalankan", "execute"],
      ["ambil_env", "getenv"],
      ["hapus", "remove"],
      ["ganti_nama", "rename"],
      ["set_lokal", "setlocale"],
      ["file_sementara", "tmpname"]
    ]);

    lua.lua_getglobal(L, to_luastring("io"));
    if (lua.lua_istable(L, -1)) {
      const ioAliases = [
        ["tulis", "write"],
        ["baca", "read"],
        ["buka", "open"],
        ["tutup", "close"],
        ["siram", "flush"],
        ["baris", "lines"],
        ["masukan", "input"],
        ["keluaran", "output"],
        ["tipe", "type"]
      ];
      for (let j = 0; j < ioAliases.length; j++) {
        lua.lua_getfield(L, -1, to_luastring(ioAliases[j][1]));
        if (!lua.lua_isnil(L, -1)) {
          lua.lua_setfield(L, -2, to_luastring(ioAliases[j][0]));
        } else {
          lua.lua_pop(L, 1);
        }
      }
    }
    lua.lua_pop(L, 1);

    copyModule("coroutine", "korutin", [
      ["buat", "create"],
      ["lanjut", "resume"],
      ["hasil", "yield"],
      ["status", "status"],
      ["bungkus", "wrap"],
      ["berjalan", "running"],
      ["bisa_hasil", "isyieldable"],
      ["tutup", "close"]
    ]);

    return L;
  }

  function execute(source, onPrint, onRead) {
    const f = getFengari();
    if (!f) {
      return {
        success: false,
        error: "Engine Lua Fengari tidak tersedia.",
        elapsedMs: 0
      };
    }

    const lua = f.lua;
    const lauxlib = f.lauxlib;
    const to_luastring = f.to_luastring;
    const to_jsstring = f.to_jsstring;

    const startTime = Date.now();
    let L = null;

    try {
      L = initRuntime(onPrint, onRead);
      const transpiled = transpile(source);
      const status = lauxlib.luaL_dostring(L, to_luastring(transpiled));
      const elapsedMs = Date.now() - startTime;

      if (status !== lua.LUA_OK) {
        let errStr = "Galat eksekusi tidak dikenal.";
        if (lua.lua_isstring(L, -1)) {
          errStr = to_jsstring(lua.lua_tostring(L, -1));
        }
        return {
          success: false,
          error: errStr,
          elapsedMs: elapsedMs
        };
      }

      return {
        success: true,
        elapsedMs: elapsedMs
      };
    } catch (e) {
      const elapsedMs = Date.now() - startTime;
      return {
        success: false,
        error: e.message || String(e),
        elapsedMs: elapsedMs
      };
    }
  }

  return {
    transpile: transpile,
    initRuntime: initRuntime,
    execute: execute
  };
});
