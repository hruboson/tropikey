{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "libtropic-dev-shell";

  buildInputs = with pkgs; [
	gcc
	gdb
	cmake

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
    echo "  [1/2] Toolchain ..... gcc, cmake ready"

	if [ -f "CMakeLists.txt" ]; then
		if [ -f "compile_commands.json" ]; then
		  	echo "  [2/2] compile_commands ..... already exists, skipping"
		else
			echo "  [2/2] compile_commands .... generating..."
		  	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null 2>&1
		  	ln -sf build/compile_commands.json compile_commands.json
			echo "  [2/2] compile_commands ..... compile_commands.json ready"
		fi
	else
	    echo "  [2/2] compile_commands .... no CMakeLists.txt found, skipping"
	fi

	echo ""
	echo "  ready. happy coding 🌴"
	echo ""
  '';
}
