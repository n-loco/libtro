tests = [
    'rune_to_u8',
    'rune_to_u16',
    'u8_to_rune',
    'u16_to_rune',
    'str8_rune_len',
    'str16_rune_len',
]

foreach test_name : tests
    t = executable(
        test_name,
        test_name + '.c',
        include_directories: general_test_include_dirs,
        link_with: general_test_libs_to_link,
    )
    test(test_name, t, suite: 'uchar')
endforeach
