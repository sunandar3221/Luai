class LuaiAcodePlugin {
  constructor() {
    this.baseUrl = "";
    this.modal = null;
    this.consoleOutput = null;
    this.engineLoaded = false;
    this.runBtn = null;
  }

  async init($page, cacheFileUrl, cacheFile) {
    this.registerAceMode();
    await this.loadEngine();
    this.registerCommands();
    this.registerHeaderButton();
  }

  registerAceMode() {
    if (typeof ace === "undefined") return;

    ace.define(
      "ace/mode/luai_highlight_rules",
      ["require", "exports", "module", "ace/lib/oop", "ace/mode/text_highlight_rules"],
      function (require, exports, module) {
        const oop = require("../lib/oop");
        const TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;

        const LuaiHighlightRules = function () {
          const keywords = (
            "jika|maka|atau_jika|selain_itu|selesai|untuk|selama|ulang|sampai|berhenti|lakukan|lompat_ke|kembalikan"
          );
          const storageTypes = (
            "fungsi|lokal"
          );
          const builtinConstants = (
            "benar|salah|kosong|nihil"
          );
          const logicalOperators = (
            "dan|atau|bukan|dalam"
          );
          const builtinFunctions = (
            "cetak|tipe|ke_angka|ke_teks|pasangan|i_pasangan|masukan|lepas|tegaskan|kesalahan|panggil_aman|pcall_aman|xpcall_aman|set_metatabel|ambil_metatabel|koleksi_sampah|butuh|perlu|pilih|muat|muat_file|eksekusi_file"
          );
          const builtinLibraries = (
            "matematika|tabel|teks|sistem|io|korutin"
          );

          const keywordMapper = this.createKeywordMapper(
            {
              "keyword.control": keywords,
              "storage.type": storageTypes,
              "constant.language": builtinConstants,
              "keyword.operator": logicalOperators,
              "support.function": builtinFunctions,
              "support.class": builtinLibraries,
              "variable.language": "self"
            },
            "identifier",
            true
          );

          this.$rules = {
            start: [
              {
                token: "comment",
                regex: "--\\[(=*)\\[[\\s\\S]*?\\]\\1\\]"
              },
              {
                token: "comment",
                regex: "--.*$"
              },
              {
                token: "string",
                regex: "\\[(=*)\\[[\\s\\S]*?\\]\\1\\]"
              },
              {
                token: "string",
                regex: '".*?"'
              },
              {
                token: "string",
                regex: "'.*?'"
              },
              {
                token: "constant.numeric",
                regex: "0[xX][0-9a-fA-F]+(\\.[0-9a-fA-F]+)?([pP][+-]?[0-9]+)?\\b"
              },
              {
                token: "constant.numeric",
                regex: "[+-]?\\d+(?:(?:\\.\\d*)?(?:[eE][+-]?\\d+)?)?\\b"
              },
              {
                token: "keyword.operator",
                regex: "\\+|\\-|\\*|\\/\\/|\\/|\\%|\\^|==|~=|<|>|<=|>=|\\.\\.\\.|\\.\\.|#|&|\\||~|>>|<<"
              },
              {
                token: keywordMapper,
                regex: "[a-zA-Z_$][a-zA-Z0-9_$]*\\b"
              },
              {
                token: "paren.lparen",
                regex: "[\\[\\(\\{]"
              },
              {
                token: "paren.rparen",
                regex: "[\\]\\)\\}]"
              },
              {
                token: "text",
                regex: "\\s+"
              }
            ]
          };
          this.normalizeRules();
        };

        oop.inherits(LuaiHighlightRules, TextHighlightRules);
        exports.LuaiHighlightRules = LuaiHighlightRules;
      }
    );

    ace.define(
      "ace/mode/luai",
      ["require", "exports", "module", "ace/lib/oop", "ace/mode/text", "ace/mode/luai_highlight_rules"],
      function (require, exports, module) {
        const oop = require("../lib/oop");
        const TextMode = require("./text").Mode;
        const LuaiHighlightRules = require("./luai_highlight_rules").LuaiHighlightRules;

        const Mode = function () {
          this.HighlightRules = LuaiHighlightRules;
          this.$behaviour = this.$defaultBehaviour;
          this.lineCommentStart = "--";
          this.blockComment = { start: "--[[", end: "]]" };
        };
        oop.inherits(Mode, TextMode);

        exports.Mode = Mode;
      }
    );

    try {
      const modelist = ace.require("ace/ext/modelist");
      if (modelist) {
        const luaiMode = {
          name: "luai",
          caption: "Luai",
          mode: "ace/mode/luai",
          extensions: "luai",
          extRe: /^luai$/i
        };
        modelist.modesByName.luai = luaiMode;
        modelist.modes.push(luaiMode);
      }
    } catch (e) {}
  }

  async loadEngine() {
    if (this.engineLoaded && window.LuaiEngine) return;

    const loadScript = (src) => {
      return new Promise((resolve, reject) => {
        const script = document.createElement("script");
        script.src = src;
        script.onload = () => resolve();
        script.onerror = () => reject(new Error("Gagal memuat: " + src));
        document.head.appendChild(script);
      });
    };

    try {
      if (typeof fengari === "undefined") {
        await loadScript(this.baseUrl + "fengari-web.js");
      }
      if (typeof window.LuaiEngine === "undefined") {
        await loadScript(this.baseUrl + "luai-engine.js");
      }
      this.engineLoaded = true;
    } catch (e) {
      console.error("Gagal menginisialisasi LuaiEngine:", e);
    }
  }

  registerCommands() {
    if (typeof editorManager === "undefined") return;

    if (window.acode && acode.addCommand) {
      acode.addCommand({
        name: "luai_run",
        description: "Jalankan Berkas Luai (Run Luai File)",
        exec: () => this.runActiveFile()
      });

      acode.addCommand({
        name: "luai_repl",
        description: "Buka Terminal Interaktif REPL Luai",
        exec: () => this.openRepl()
      });
    }
  }

  registerHeaderButton() {
    if (typeof editorManager === "undefined") return;

    const checkFile = () => {
      const activeFile = editorManager.activeFile;
      if (!activeFile) return;

      const isLuai = activeFile.name && activeFile.name.endsWith(".luai");
      if (isLuai) {
        this.showFloatingRunBtn();
      } else {
        this.hideFloatingRunBtn();
      }
    };

    editorManager.on("switch-file", checkFile);
    editorManager.on("rename-file", checkFile);
    checkFile();
  }

  showFloatingRunBtn() {
    if (this.runBtn) {
      this.runBtn.style.display = "flex";
      return;
    }

    const btn = document.createElement("button");
    btn.id = "luai-run-fab";
    btn.innerHTML = "&#9654;";
    btn.title = "Jalankan Luai";
    btn.style.cssText = [
      "position: fixed",
      "bottom: 80px",
      "right: 20px",
      "width: 48px",
      "height: 48px",
      "border-radius: 50%",
      "background-color: #22c55e",
      "color: #ffffff",
      "border: none",
      "font-size: 20px",
      "box-shadow: 0 4px 12px rgba(0,0,0,0.4)",
      "display: flex",
      "align-items: center",
      "justify-content: center",
      "z-index: 9999",
      "cursor: pointer"
    ].join(";");

    btn.addEventListener("click", () => this.runActiveFile());
    document.body.appendChild(btn);
    this.runBtn = btn;
  }

  hideFloatingRunBtn() {
    if (this.runBtn) {
      this.runBtn.style.display = "none";
    }
  }

  ensureConsole() {
    if (this.modal) return;

    const modal = document.createElement("div");
    modal.id = "luai-console-modal";
    modal.style.cssText = [
      "position: fixed",
      "top: 0",
      "left: 0",
      "width: 100vw",
      "height: 100vh",
      "background: rgba(0,0,0,0.6)",
      "z-index: 10000",
      "display: none",
      "flex-direction: column",
      "align-items: center",
      "justify-content: center",
      "padding: 16px",
      "box-sizing: border-box"
    ].join(";");

    const card = document.createElement("div");
    card.style.cssText = [
      "width: 100%",
      "max-width: 600px",
      "height: 80vh",
      "background: #111827",
      "border-radius: 12px",
      "border: 1px solid #374151",
      "display: flex",
      "flex-direction: column",
      "overflow: hidden",
      "box-shadow: 0 10px 25px rgba(0,0,0,0.5)"
    ].join(";");

    const header = document.createElement("div");
    header.style.cssText = [
      "background: #1f2937",
      "padding: 12px 16px",
      "display: flex",
      "justify-content: space-between",
      "align-items: center",
      "border-bottom: 1px solid #374151"
    ].join(";");

    const titleBox = document.createElement("div");
    titleBox.style.cssText = "display: flex; align-items: center; gap: 8px;";
    titleBox.innerHTML = "<span style='color: #22c55e; font-weight: bold;'>&#9654; Luai Terminal</span>";

    const actions = document.createElement("div");
    actions.style.cssText = "display: flex; gap: 8px;";

    const copyBtn = document.createElement("button");
    copyBtn.innerText = "Salin";
    copyBtn.style.cssText = "background: #374151; color: #fff; border: none; padding: 6px 12px; border-radius: 6px; font-size: 12px; cursor: pointer;";
    copyBtn.onclick = () => {
      if (this.consoleOutput) {
        if (navigator.clipboard) {
          navigator.clipboard.writeText(this.consoleOutput.innerText);
        }
        if (window.toast) toast("Output disalin!");
      }
    };

    const clearBtn = document.createElement("button");
    clearBtn.innerText = "Bersihkan";
    clearBtn.style.cssText = "background: #374151; color: #fff; border: none; padding: 6px 12px; border-radius: 6px; font-size: 12px; cursor: pointer;";
    clearBtn.onclick = () => {
      if (this.consoleOutput) {
        this.consoleOutput.innerHTML = "";
      }
    };

    const closeBtn = document.createElement("button");
    closeBtn.innerText = "Tutup";
    closeBtn.style.cssText = "background: #ef4444; color: #fff; border: none; padding: 6px 12px; border-radius: 6px; font-size: 12px; cursor: pointer;";
    closeBtn.onclick = () => {
      modal.style.display = "none";
    };

    actions.appendChild(copyBtn);
    actions.appendChild(clearBtn);
    actions.appendChild(closeBtn);

    header.appendChild(titleBox);
    header.appendChild(actions);

    const body = document.createElement("div");
    body.id = "luai-console-body";
    body.style.cssText = [
      "flex: 1",
      "padding: 14px",
      "background: #0f172a",
      "color: #f8fafc",
      "font-family: monospace",
      "font-size: 13px",
      "line-height: 1.5",
      "overflow-y: auto",
      "white-space: pre-wrap",
      "word-break: break-all"
    ].join(";");

    card.appendChild(header);
    card.appendChild(body);
    modal.appendChild(card);
    document.body.appendChild(modal);

    this.modal = modal;
    this.consoleOutput = body;
  }

  async runActiveFile() {
    this.ensureConsole();
    this.modal.style.display = "flex";
    this.consoleOutput.innerHTML = "<span style='color: #64748b;'>Menjalankan kode Luai...</span>\n\n";

    await this.loadEngine();

    let code = "";
    if (typeof editorManager !== "undefined" && editorManager.activeFile) {
      code = editorManager.activeFile.session.getValue();
    }

    if (!code || code.trim().length === 0) {
      this.consoleOutput.innerHTML += "<span style='color: #f59e0b;'>[Peringatan]: Berkas aktif kosong.</span>\n";
      return;
    }

    const onPrint = (text) => {
      const line = document.createElement("div");
      line.textContent = text;
      line.style.color = "#f8fafc";
      this.consoleOutput.appendChild(line);
      this.consoleOutput.scrollTop = this.consoleOutput.scrollHeight;
    };

    const res = window.LuaiEngine.execute(code, onPrint);

    if (res.success) {
      const footer = document.createElement("div");
      footer.style.cssText = "color: #22c55e; margin-top: 12px; font-weight: bold;";
      footer.textContent = "\n[Program selesai dieksekusi dalam " + res.elapsedMs + " ms]";
      this.consoleOutput.appendChild(footer);
    } else {
      const errBox = document.createElement("div");
      errBox.style.cssText = "color: #ef4444; margin-top: 12px; font-weight: bold;";
      errBox.textContent = "\n[Galat Runtime/Sintaks]: " + res.error;
      this.consoleOutput.appendChild(errBox);
    }
  }

  openRepl() {
    this.ensureConsole();
    this.modal.style.display = "flex";
    this.consoleOutput.innerHTML = "<span style='color: #22c55e; font-weight: bold;'>Luai 1.0.0 Interactive REPL</span>\n";
    this.consoleOutput.innerHTML += "<span style='color: #94a3b8;'>Gunakan tombol FAB atau ketik ekspresi Luai.</span>\n\n";
  }

  destroy() {
    if (this.modal) {
      this.modal.remove();
      this.modal = null;
    }
    if (this.runBtn) {
      this.runBtn.remove();
      this.runBtn = null;
    }
  }
}

if (window.acode) {
  const luaiPlugin = new LuaiAcodePlugin();
  acode.setPluginInit("com.luailang.acode", (baseUrl, $page, { cacheFileUrl, cacheFile }) => {
    if (!baseUrl.endsWith("/")) {
      baseUrl += "/";
    }
    luaiPlugin.baseUrl = baseUrl;
    luaiPlugin.init($page, cacheFileUrl, cacheFile);
  });
  acode.setPluginUnmount("com.luailang.acode", () => {
    luaiPlugin.destroy();
  });
}
