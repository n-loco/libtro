sources += files(
    'file_share.c',
    'outinerr.c',
)

if system_name == 'windows'
    sources += files(
        'file_win32.c',
    )
endif

if is_unix_like
    sources += files(
        'file_unix.c',
    )
endif
