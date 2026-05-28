project(
    'libtro',
    'c',
    # Veja: tools/bin
    version: run_command('version', check: true).stdout(),
    meson_version: '>= 1.10.0',
    default_options: [
        'warning_level=3',
        'c_std=c11',
        'b_lto=false',  # Garantia de que os símbolos não vão sumir.
    ],
    subproject_dir: 'vendor',
    license: 'BSD 3-Clause',
    license_files: 'LICENSE',
)

c_compiler = meson.get_compiler('c')
c_syntax_msvc = c_compiler.get_argument_syntax() == 'msvc'

# Útil para fazer compilação condicional
# direto pelo Meson, selecionando arquivos.
# https://mesonbuild.com/Reference-tables.html#operating-system-names
system_name = host_machine.system()
if system_name == 'cygwin'
    error('Cygwin and MSYS are not currently supported.')
endif

# Por algum motivo Meson não tem
# uma flag pronta para dizer se um
# sistema é tipo Unix ou não, então
# nós testamos se isto aqui compila.
unix_like_test_code = '''
#include <tro/conditionals.h>
#if !TRO_SYSTEM_UNIX_LIKE
# error "not Unix-like."
#endif
'''
# Também útil para compilação condicional.
is_unix_like = c_compiler.compiles(
    unix_like_test_code,
    name: 'is Unix-like',
    include_directories: ['include'],
)
unset_variable('unix_like_test_code')

# Lista de argumentos com sintaxe tipo MSVC.
project_args_msvc_syntax = [
    '/utf-8',
    # Existe um bug no Visual Studio 2022
    # que é um falso positivo para o warning C4200,
    # então nós desabilitamos este warning :^)
    '/wd4200',
]
# Lista de argumentos com sintaxe tipo GCC (GCC, Clang, etc.).
project_args_gcc_syntax = [
    '-finput-charset=utf-8',
    '-fexec-charset=utf-8',
]

if is_unix_like
    # Versão do padrão POSIX que
    # queremos ter acesso às funções.
    posix_c_source = '200809L'

    # Usar um compilador com sintaxe do MSVC em uma máquina
    # tipo Unix é bem improvável, mas não custa nada deixar
    # os argumentos sincronizados.
    project_args_msvc_syntax += '/D_POSIX_C_SOURCE=' + posix_c_source
    project_args_gcc_syntax += '-D_POSIX_C_SOURCE=' + posix_c_source
endif

if c_syntax_msvc
    add_project_arguments(project_args_msvc_syntax, language: 'c')
else
    add_project_arguments(project_args_gcc_syntax, language: 'c')
endif

# Biblioteca com algoritmo de
# conversão de float para string.
ryu_dep = dependency('ryu')

# Em sistemas como Linux, a
# biblioteca de matemática
# (math.h) é um binário aparte.
m_dep = c_compiler.find_library('m', required: false)

dependencies = [
    ryu_dep,
    m_dep,
]

# Lista de includes que projetos externos usam.
public_includes = []
# Lista de includes geral do projeto usados durante o build.
project_includes = []

sources = []

subdir('include')
subdir('src')

lib_name_prefix = []
lib_name_suffix = []
if c_syntax_msvc
    lib_name_prefix = ''
    lib_name_suffix = 'lib'
endif

# Para fácil criação de programas
# com a libtro, ela é SEMPRE estática.
lib = static_library(
    'tro',
    sources,
    include_directories: project_includes,
    install: true,
    gnu_symbol_visibility: 'hidden',
    dependencies: dependencies,
    name_prefix: lib_name_prefix,
    name_suffix: lib_name_suffix,
)

subdir('tests')

# Importante para build-systems serem capazes
# de encontrar esta biblioteca automatiamente,
# especialmente em distribuições de Linux.
if not c_syntax_msvc
    pkg_mod = import('pkgconfig')
    pkg_mod.generate(
        lib,
        filebase: 'tro',
        name: 'LibTRO',
        # Manter sincronizado com o Doxyfile, com o
        # README.md e com a descrição no GitHub.
        description: 'Uma biblioteca com funções semelhantes às do C para fins educacionais na Eletrônica, IF Sul Pelotas.',
    )
endif

# Isto aqui é coisa específica do Meson.
# Não é necessário se preocupar no geral,
# serve para outros projetos Meson serem
# capazes de usar este repositório como
# uma dependência diretamente.
tro_dep = declare_dependency(
    include_directories: public_includes,
    dependencies: dependencies,
    link_with: [lib],
)
meson.override_dependency('tro', tro_dep)
