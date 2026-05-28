# Configurações #
#===============#

set unstable := true
set windows-shell := ["cmd", "/c"]
set shell := ["sh", "-c"]

# Dependências #
#==============#

python := if os() == "windows" { require("python") } else { require("python3") }
meson := require("meson")
clang-format := which("clang-format")
doxygen := which("doxygen")

# Opções #
#========#

build-dir := env("MESON_BUILDDIR", justfile_dir() / "build")

# Ambiente Exportado #
#====================#

export PATH := (justfile_dir() / 'tools' / 'bin') + PATH_VAR_SEP + env("PATH")
export CLANG_FORMAT_PATH := clang-format
export MESON_PATH := meson
export PYTHON3_PATH := python
export MESON_BUILDDIR := build-dir
export MESON_CROSS_DIR := justfile_dir() / "tools" / "meson" / "cross"
export PYTHONPATH := justfile_dir() / "tools" / "lib"

# Tarefas #
#=========#

# Documenta o Justfile.
[arg("term", help="Opcional, para obter ajuda sobre um recipe em específico.")]
[default]
@help term="":
    just {{ if term == "" { "--list" } else { "--usage " + term } }}
    echo {{ if term == "" { BOLD + MAGENTA + "DICA:" + NORMAL + " Use: just help help." } else { "" } }}

# Cria setup do projeto Meson.
setup:
    {{ meson }} setup --reconfigure {{ build-dir }}

# Faz o build da biblioteca e dos testes.
build:
    {{ meson }} compile -j 6 -C {{ build-dir }}

# Executa testes Meson.
[arg("args", help="Uma lista opcional de argumentos para o Meson.")]
test *args: build
    {{ meson }} test --no-rebuild -C {{ build-dir }} {{ args }}

# Lista testes disponíveis.
list_tests: setup
    {{ meson }} test --no-rebuild --list -C {{ build-dir }}

# Formata os arquivos do projeto (requer Clang Format).
format:
    {{ python }} {{ justfile_dir() / 'tools' / 'scripts' / 'format.py' }}

# Roda um script de configuração.
[arg("tool", help="A ferramenta. (valores: CodeLLDB, Meson+MSYS)")]
config tool:
    {{ python }} {{ justfile_dir() / 'tools' / 'configs' / tool + '.py' }}

# Gera documentação (requer Doxygen).
document:
    {{ if doxygen == '' { error('Doxygen não encontrado') } else { doxygen } }}

# Recipe apenas para CI (mais informações em tools/scripts/ci/README.md).
ci job *args:
    {{ python }} {{ justfile_dir() / 'tools' / 'scripts' / 'ci' / job + '.py' }} {{ args }}
