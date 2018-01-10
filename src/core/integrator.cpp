#include "../../include/core/integrator.h"
#include "../../include/integrator/pathtracer.h"

void Integrator::render(const Scene& scene)
{

}

Integrator::ptr Integrator::load_from_json(const nlohmann::json& in)
{
  return Integrator::ptr( new Pathtracer );
}
