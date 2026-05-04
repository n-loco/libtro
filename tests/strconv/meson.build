tests = [
    'float2str',
    'float2str16',
    'int2str',
    'int2str16',
]

foreach test_name : tests
    t = executable(
        test_name,
        test_name + '.c',
        include_directories: general_test_include_dirs,
        link_with: general_test_libs_to_link,
    )
    test('strconv/' + test_name, t)
endforeach
