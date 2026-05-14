# Configurações #
#===============#

set unstable := true
set windows-shell := ["cmd", "/c"]
set shell := ["sh", "-c"]

# Dependências #
#==============#

python := if os() == "windows" { require("python") } else { require("python3") }
meson := require("meson")
clang-format := require("clang-format")
doxygen := which('doxygen')

# Opções #
#========#

build-dir := env("MESON_BUILDDIR", "build")

# Ambiente Exportado #
#====================#

export CLANG_FORMAT_PATH := clang-format
export MESON_PATH := meson
export PYTHONPATH := "tools/lib"
export MESON_BUILDDIR := build-dir

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

# Formata os arquivos do projeto.
format:
    {{ python }} tools/scripts/format.py

# Roda um script de configuração.
[arg("tool", help="A ferramenta. (valores: CodeLLDB)")]
config tool: setup
    {{ python }} tools/configs/{{ tool }}.py

# Gera documentação (depende do Doxygen).
document:
    {{ if doxygen == "" { error("Doxygen não encontrado") } else { doxygen } }}
