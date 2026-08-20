// SkyWatch-20 Divine Easter Egg Component (HolyC Dialect)
// Activation Trigger: Type "RETRORADAR" into the terminal prompt

U0 SpawnSpaceInvaders()
{
  I64 matrix_x = 0;
  I64 matrix_y = 0;

  "👾 [DIVINE ARCHAEOLOGY ACTIVE] 👾\n";
  "Preparing classic sector defensive grids...\n\n";

  for (matrix_y = 0; matrix_y < 4; matrix_y++) {
    Print(" | ");
    for (matrix_x = 0; matrix_x < 10; matrix_x++) {
      // Draw rows of attacking alien glyph space targets
      Print("👽 ");
    }
    Print("|\n");
  }
  
  Print("\n         ▲   [DEFENSE TURRET ALIGNED]\n");
}
