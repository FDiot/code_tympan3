#include "Tympan/models/common/acoustic_path.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"


namespace Tools
{
/**
 * \brief convert an event from geometric ray to an acoustic_path event
 */
acoustic_event build_from_RayEvent(const Event *rev);

/**
 * \brief build a list of acoustic_events of a given acoustic_path from a geometric ray
 */
void build_event_list_from_Ray(int sens, Ray* ray, acoustic_path& ap);

/**
 * \brief build an acoustic_path from a geometric ray
 */
acoustic_path build_from_Ray(int sens, Ray* ray);
}
