// SkyWatch-20 TCAS Collision Avoidance Module (HolyC Dialect)
// Monitors airspace matrices and chirps internal PC speakers on violation checks

#include "HolyRadarCore.HC"

#define CRITICAL_THRESHOLD_NM 5.0

U0 AuditDivineSeparation(CAircraftNode *ac1, CAircraftNode *ac2)
{
  F64 separation_distance = ComputeHolyDistance(&ac1->position, &ac2->position);
  
  if (separation_distance <= CRITICAL_THRESHOLD_NM) {
    Sound(880); 
    MDelay(200); 
    Sound(0);
    Print("⚠️ [DIVINE ALARM] Critical separation broken!");
  }
}
