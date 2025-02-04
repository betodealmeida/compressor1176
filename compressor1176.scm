(use-modules (guix packages)
             (guix git-download)
             (guix build-system cmake)
             (guix licenses)
             (gnu packages)
             (gnu packages pkg-config)
             (gnu packages audio))

(define-public compressor1176
               (package
                 (name "compressor1176")
                 (version "0.1.0")
                 (source (origin
                           (method git-fetch)
                           (uri (git-reference
                                  (url "https://github.com/betodealmeida/compressor1176")
                                  (commit "main")))
                           (file-name (git-file-name name version))
                           (sha256
                             (base32
                               "0xc8lxk5x1rlhxcw25416mpy7xbpx764grdn956k5r7na0g1w3yk"))))
                 (build-system cmake-build-system)
                 (arguments
                   '(#:tests? #f))  ; No tests
                 (native-inputs (list pkg-config))
                 (inputs (list (specification->package "lv2")))
                 (home-page "https://github.com/betodealmeida/compressor1176")
                 (synopsis "1176 compressor emulation as LV2 plugin")
                 (description "LV2 plugin emulating the classic 1176 FET compressor.")
                 (license gpl3+)))

compressor1176
