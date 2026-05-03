{
	description = "Tropikey management tool and PKCS#11 module";

	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
	};

	outputs = { self, nixpkgs }: 
		let
		systems = [ "x86_64-linux" "aarch64-linux" ];
	forAllSystems = nixpkgs.lib.genAttrs systems;
	in {
		packages = forAllSystems (system:
			let
				pkgs = import nixpkgs { inherit system; };
				libtropic-src = pkgs.fetchFromGitHub {
					owner  = "tropicsquare";
					repo   = "libtropic";
					rev    = "51044cdc2e0aabff42305130b344c5db3136f158";
					hash   = "sha256-Ap+wa05RgDO0UMofBBmhDQ30dLKUmYDlpJ1FvqTgEhU=";
				};
			in {
			tropikey = pkgs.stdenv.mkDerivation {
				pname = "tropikey";
				version = "0.1.0";

				src = ./.;
				postUnpack = ''
					cp -r ${libtropic-src} $sourceRoot/libtropic
					chmod -R u+w $sourceRoot/libtropic
				'';

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
				];

				cmakeFlags = [
					"-DCMAKE_BUILD_TYPE=Release"
				];

				installPhase = ''
					mkdir -p $out/lib
					mkdir -p $out/bin

					cp tropikey_pkcs11.so $out/lib/
					cp tropikey $out/bin/
					'';
			};

			default = self.packages.${system}.tropikey;
		});

		nixosModules.default = import ./nix/tropikey-module.nix self;

		apps = forAllSystems (system: {
			default = {
			type = "app";
			program = "${self.packages.${system}.default}/bin/tropikey";
			};
		});
	};
}
