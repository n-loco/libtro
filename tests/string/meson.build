tests = [
    'sfmt',
    'fmt_parser',
    'fmt16_parser',
    'rune_to_u8',
    'rune_to_u16',
    'u8_to_rune',
    'u16_to_rune',
    'str8_rune_len',
    'str16_rune_len',
    'str8_to_str16',
    'str16_to_str8',
    'strdybuf',
    'str16dybuf',
]

foreach test_name : tests
    t = executable(
        test_name,
        test_name + '.c',
        include_directories: general_test_include_dirs,
        link_with: general_test_libs_to_link,
    )
    test('string/' + test_name, t)
endforeach
