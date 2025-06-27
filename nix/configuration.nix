{ inputs, ... }: {

  imports = [
    inputs.hydra-coding-standards.flakeModule
  ];

  perSystem.coding.standards.hydra.enable = true;

}
