//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "object/particlesystem.hpp"

#include <math.h>

#include "supertux/globals.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"
#include "video/drawing_context.hpp"
#include "video/surface_batch.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

ParticleSystem::ParticleSystem(const ReaderMapping& reader, float max_particle_size_) :
  GameObject(reader),
  ExposedObject<ParticleSystem, scripting::ParticleSystem>(this),
  max_particle_size(max_particle_size_),
  z_pos(LAYER_BACKGROUND1),
  particles(),
  virtual_width(static_cast<float>(SCREEN_WIDTH) + max_particle_size * 2.0f),
  virtual_height(static_cast<float>(SCREEN_HEIGHT) + max_particle_size * 2.0f),
  enabled(true)
{
  reader.get("enabled", enabled, true);
  z_pos = reader_get_layer(reader, /* default = */ LAYER_BACKGROUND1);
}

ParticleSystem::ParticleSystem(float max_particle_size_) :
  GameObject(),
  ExposedObject<ParticleSystem, scripting::ParticleSystem>(this),
  max_particle_size(max_particle_size_),
  z_pos(LAYER_BACKGROUND1),
  particles(),
  virtual_width(static_cast<float>(SCREEN_WIDTH) + max_particle_size * 2.0f),
  virtual_height(static_cast<float>(SCREEN_HEIGHT) + max_particle_size * 2.0f),
  enabled(true)
{
}

ObjectSettings
ParticleSystem::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_int(_("Z-pos"), &z_pos, "z-pos", LAYER_BACKGROUND1);
  result.add_remove();

  return result;
}

ParticleSystem::~ParticleSystem()
{
}

void
ParticleSystem::draw(DrawingContext& context)
{
  if (!enabled)
    return;

  float scrollx = context.get_translation().x;
  float scrolly = context.get_translation().y;

  context.push_transform();
  context.set_translation(Vector(max_particle_size,max_particle_size));

  std::unordered_map<SurfacePtr, SurfaceBatch> batches;
  for (const auto& particle : particles)
  {
    // remap x,y coordinates onto screencoordinates
    Vector pos;

    pos.x = fmodf(particle->pos.x - scrollx, virtual_width);
    if (pos.x < 0) pos.x += virtual_width;

    pos.y = fmodf(particle->pos.y - scrolly, virtual_height);
    if (pos.y < 0) pos.y += virtual_height;

    //if(pos.x > virtual_width) pos.x -= virtual_width;
    //if(pos.y > virtual_height) pos.y -= virtual_height;

    auto it = batches.find(particle->texture);
    if (it == batches.end()) {
      const auto& batch_it = batches.emplace(particle->texture, SurfaceBatch(particle->texture));
      batch_it.first->second.draw(pos, particle->angle);
    } else {
      it->second.draw(pos, particle->angle);
    }
  }

  for(auto& it : batches) {
    auto& surface = it.first;
    auto& batch = it.second;
    context.color().draw_surface_batch(surface,
                                       std::move(batch).get_srcrects(),
                                       std::move(batch).get_dstrects(),
                                       std::move(batch).get_angles(),
                                       Color::WHITE, z_pos);
  }

  context.pop_transform();
}

void
ParticleSystem::set_enabled(bool enabled_)
{
  enabled = enabled_;
}

bool
ParticleSystem::get_enabled() const
{
  return enabled;
}

/* EOF */
