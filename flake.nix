{
  description = "Tropikey management tool and PKCS#11 module";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    libtropic = {
      url = "github:tropicsquare/libtropic/51044cdc2e0aabff42305130b344c5db3136f158";
      flake = false;
    };
    ftxui = {
      url = "github:ArthurSonzogni/FTXUI/485a1758f08ecbd53a30f5514058dc270dede011";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      libtropic,
      ftxui,
    }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          tropikey = pkgs.stdenv.mkDerivation {
            pname = "tropikey";
            version = "0.1.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              gcc
              gdb
            ];

            buildInputs = with pkgs; [
              libsodium
              mbedtls
              openssh
              opensc

              libx11
              libxcb
            ];

            cmakeFlags = [
              "-DCMAKE_BUILD_TYPE=Release"
              "-DLIBTROPIC_SRC=${libtropic}"
              "-DFTXUI_SRC=${ftxui}"
            ];

            installPhase = /* bash */ ''
              mkdir -p $out/lib
              mkdir -p $out/bin

              cp tropikey_pkcs11.so $out/lib/
              cp tropikey $out/bin/
            '';
          };

          default = self.packages.${system}.tropikey;
        }
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            name = "tropikey-dev-shell";
            buildInputs = with pkgs; [
              gcc
              gdb
              cmake
              mbedtls
              libsodium
              openssh
              opensc
              pkg-config
              doxygen

              libx11
              libxcb
            ];
            shellHook = /* bash */ ''
              echo "╔═════════════════════════════╗"
              echo "║ tropikey development shell  ║"
              echo "╚═════════════════════════════╝"
              echo ""
              echo "  [1/3] Toolchain ..... gcc, gdb, cmake ready"
              echo "  [2/3] SSH utils ..... ssh, pkcs11 ready"
              if [ -f "CMakeLists.txt" ]; then
              	echo "  [3/3] compile_commands .... generating..."
              	cmake -S . -B build \
              		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
              		-DLIBTROPIC_SRC=${libtropic} \
              		-DFTXUI_SRC=${ftxui} \
              		> /dev/null 2>&1
              	ln -sf build/compile_commands.json compile_commands.json
              	echo "  [3/3] compile_commands ..... compile_commands.json ready"
              else
              	echo "  [3/3] compile_commands .... no CMakeLists.txt found, skipping"
              fi
              echo ""
              echo "  ready. happy coding 🌴"
              echo ""
            '';
          };
        }
      );

      nixosModules.default = import ./nix/tropikey-module.nix self;

      apps = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = {
            type = "app";
            program = "${self.packages.${system}.default}/bin/tropikey";
          };
          docs = {
            type = "app";
            program = "${pkgs.writeShellScript "generate-docs" ''
              cat > /tmp/tropikey-Doxyfile <<EOF
              $(cat Doxyfile)

              # STYLING

              HTML_EXTRA_STYLESHEET  = ${pkgs.doxygen-awesome-css}/share/doxygen-awesome-css/doxygen-awesome.css \ ${pkgs.doxygen-awesome-css}/share/doxygen-awesome-css/doxygen-awesome-sidebar-only.css

              HTML_COLORSTYLE        = DARK

              GENERATE_TREEVIEW      = YES

              DISABLE_INDEX          = NO

              FULL_SIDEBAR           = NO

              EOF
              ${pkgs.doxygen}/bin/doxygen /tmp/tropikey-Doxyfile
              xdg-open "$PWD/doc/html/index.html" # hopefully user has xdg-open installed and on path (xdg-utils is like 100MB download, I ain't adding that)
            ''}";
          };
        }
      );
    };
}
