const vscode = require('vscode');
const path = require('path');
const fs = require('fs');

function getLuaiExecutable(context) {
    const config = vscode.workspace.getConfiguration('luai');
    const customPath = config.get('executablePath');
    if (customPath && customPath.trim().length > 0 && fs.existsSync(customPath.trim())) {
        return customPath.trim();
    }

    const binaryName = process.platform === 'win32' ? 'luai.exe' : 'luai';
    const bundledPath = path.join(context.extensionPath, 'bin', binaryName);
    if (fs.existsSync(bundledPath)) {
        return bundledPath;
    }

    return 'luai';
}

let activeTerminal = null;

function getTerminal(clearBeforeRun) {
    if (activeTerminal && !activeTerminal.exitStatus) {
        if (clearBeforeRun) {
            vscode.commands.executeCommand('workbench.action.terminal.clear');
        }
        return activeTerminal;
    }

    const terminals = vscode.window.terminals;
    const existing = terminals.find(t => t.name === 'Luai');
    if (existing) {
        activeTerminal = existing;
        if (clearBeforeRun) {
            vscode.commands.executeCommand('workbench.action.terminal.clear');
        }
        return activeTerminal;
    }

    activeTerminal = vscode.window.createTerminal('Luai');
    return activeTerminal;
}

function formatCommand(exePath, args) {
    const isWindows = process.platform === 'win32';
    const quotedExe = exePath.includes(' ') ? `"${exePath}"` : exePath;
    const quotedArgs = args.map(a => a.includes(' ') || a.includes('"') ? `"${a.replace(/"/g, '\\"')}"` : a).join(' ');

    if (isWindows) {
        return `& ${quotedExe} ${quotedArgs}`.trim();
    }
    return `${quotedExe} ${quotedArgs}`.trim();
}

function activate(context) {
    const runFileDisposable = vscode.commands.registerCommand('luai.runFile', async (uri) => {
        let targetUri = uri;
        if (!targetUri) {
            const editor = vscode.window.activeTextEditor;
            if (editor) {
                targetUri = editor.document.uri;
            }
        }

        if (!targetUri || targetUri.scheme !== 'file') {
            vscode.window.showWarningMessage('Tidak ada berkas Luai (.luai) aktif untuk dijalankan.');
            return;
        }

        const editor = vscode.window.activeTextEditor;
        if (editor && editor.document.uri.toString() === targetUri.toString() && editor.document.isDirty) {
            await editor.document.save();
        }

        const filePath = targetUri.fsPath;
        const exePath = getLuaiExecutable(context);
        const config = vscode.workspace.getConfiguration('luai');
        const clearBeforeRun = config.get('clearTerminalBeforeRun', true);

        const terminal = getTerminal(clearBeforeRun);
        terminal.show(true);

        const cmd = formatCommand(exePath, [filePath]);
        terminal.sendText(cmd);
    });

    const runSelectionDisposable = vscode.commands.registerCommand('luai.runSelection', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showWarningMessage('Tidak ada editor aktif.');
            return;
        }

        const selection = editor.selection;
        const text = editor.document.getText(selection);
        if (!text || text.trim().length === 0) {
            vscode.window.showWarningMessage('Pilih kode Luai yang ingin dijalankan terlebih dahulu.');
            return;
        }

        const exePath = getLuaiExecutable(context);
        const config = vscode.workspace.getConfiguration('luai');
        const clearBeforeRun = config.get('clearTerminalBeforeRun', false);

        const terminal = getTerminal(clearBeforeRun);
        terminal.show(true);

        const oneLiner = text.replace(/\r?\n/g, ' ');
        const cmd = formatCommand(exePath, ['-e', oneLiner]);
        terminal.sendText(cmd);
    });

    const startReplDisposable = vscode.commands.registerCommand('luai.startRepl', () => {
        const exePath = getLuaiExecutable(context);
        const terminal = vscode.window.createTerminal('Luai REPL');
        terminal.show(true);

        const cmd = formatCommand(exePath, []);
        terminal.sendText(cmd);
    });

    context.subscriptions.push(runFileDisposable);
    context.subscriptions.push(runSelectionDisposable);
    context.subscriptions.push(startReplDisposable);
}

function deactivate() {
    activeTerminal = null;
}

module.exports = {
    activate,
    deactivate
};
