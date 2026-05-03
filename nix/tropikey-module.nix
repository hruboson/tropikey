self: { config, lib, pkgs, ... }:

let
	cfg = config.programs.tropikey;
	system = pkgs.stdenv.hostPlatform.system;
	tropikeyPkg = self.packages.${system}.tropikey;
in {
	options.programs.tropikey = {
		enable = lib.mkEnableOption "Tropikey PKCS#11 module and CLI tool";

		enableSshPkcs11 = lib.mkOption {
			type = lib.types.bool;
			default = false;
			description = ''
				Add the Tropikey PKCS#11 module to the system-wide SSH config,
				so users can use it for SSH authentication automatically.
			'';
		};
	};

	config = lib.mkIf cfg.enable {
		environment.systemPackages = [ tropikeyPkg ];

		environment.etc."tropikey/tropikey_pkcs11.so".source =
			"${tropikeyPkg}/lib/tropikey_pkcs11.so";

		# ssh config loads the module
		programs.ssh.extraConfig = lib.mkIf cfg.enableSshPkcs11 ''
			PKCS11Provider /etc/tropikey/tropikey_pkcs11.so
		'';
	};
}
