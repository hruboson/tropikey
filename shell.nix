{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "libtropic-dev-shell";

  buildInputs = with pkgs; [
	gcc
	gdb
	cmake

    openssh      
    opensc

	pkg-config
  ];

  nativeBuildInputs = with pkgs; [
	cmake
	pkg-config
  ];

  shellHook = ''
  	echo "╔═════════════════════════════╗"
    echo "║ libtropic development shell ║"
	echo "╚═════════════════════════════╝"
	echo ""
    echo "  [1/3] Toolchain ..... gcc, gdb, cmake ready"
    echo "  [2/3] SSH utils ..... ssh, pkcs11 ready"

	if [ -f "CMakeLists.txt" ]; then
		if [ -f "compile_commands.json" ]; then
		  	echo "  [3/3] compile_commands ..... already exists, skipping"
		else
			echo "  [3/3] compile_commands .... generating..."
		  	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null 2>&1
		  	ln -sf build/compile_commands.json compile_commands.json
			echo "  [3/3] compile_commands ..... compile_commands.json ready"
		fi
	else
	    echo "  [2/2] compile_commands .... no CMakeLists.txt found, skipping"
	fi

	echo ""
	echo "  ready. happy coding 🌴"
	echo ""
  '';
}
