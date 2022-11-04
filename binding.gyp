{
    "targets": [
        {
            "target_name": "binding",
            "sources": [
                "src/common.cpp",
                "src/webp_encoder.cpp",
                "src/webp.cpp",
                "src/dynamic_webp_stack.cpp",
                "src/module.cpp",
                "src/buffer_compat.cpp",
            ],
            "conditions" : [
                [
                    'OS=="linux"', {
                        "libraries" : [
                            '-L/home/pkrumins/installs/libwebp-0.6.0/lib/',
                            '-lwebp'
                        ],
                        'include_dirs': [
                            '/home/pkrumins/installs/libwebp-0.6.0/include/'
                        ],
                        'cflags!': [ '-fno-exceptions' ],
                        'cflags_cc!': [ '-fno-exceptions' ]
                    }
                ],
                [
                    'OS=="win"', {
                        "include_dirs" : [ "gyp/include", "libwebp/include" ],
                        "libraries" : [
                            '<(module_root_dir)/libwebp/lib/libwebp.lib'
                        ]
                    }
                ]
            ]
        }
    ]
}

