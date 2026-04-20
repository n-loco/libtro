project_includes += include_directories('.')
public_includes += include_directories('.')

# Todos os headers precisam ser
# listados MANUALMENTE para uma
# instalação correta da libtro.
headers = [
    'troio.h',
    'tro/conditionals.h',
    'tro/strconv.h',
    'tro/string.h',
    'tro/time.h',
    'tro/private/api.h',
    'tro/private/bool.h',
]

install_headers(headers, preserve_path: true)
