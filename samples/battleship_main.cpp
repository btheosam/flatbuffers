/*
 * Copyright 2015 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <fstream>
#include "battleship_generated.h"  // Already includes "flatbuffers/flatbuffers.h".

using namespace Battleship::Game;
using namespace std;

#define WR_FILE
#define VERIFY

// Example how to use FlatBuffers to create and read binary buffers.

int main(int /*argc*/, const char * /*argv*/[]) {
  // Build up a serialized buffer algorithmically:
  flatbuffers::FlatBufferBuilder builder;

  // First, lets serialize some weapons for the Ship: A 'RailGun' and an 'LaserGun'.
  auto weapon_one_name = builder.CreateString("RailGun");

  auto weapon_two_name = builder.CreateString("LaserGun");

  // Use the `CreateWeapon` shortcut to create Weapons with all fields set.
  auto rg = CreateWeapon(builder, weapon_one_name, Wtype_Conventional);
  auto lg = CreateWeapon(builder, weapon_two_name, Wtype_Laser);

  // Create a FlatBuffer's `vector` from the `std::vector`.
  std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
  weapons_vector.push_back(rg);
  weapons_vector.push_back(lg);
  auto weapons = builder.CreateVector(weapons_vector);

  // Second, serialize the rest of the objects needed by the Monster.
  auto position = location(63.04f, 23.43f);

  auto name = builder.CreateString("ALPHA028");

  // Shortcut for creating Battle Ship with all fields set:
  auto orc = CreateShip(builder, &position, Stype_Destroyer, name, weapons);

  builder.Finish(orc);  // Serialize the root of the object.

#ifdef WR_FILE
  uint8_t *buf = builder.GetBufferPointer();
  int len = builder.GetSize();

  std::printf("len %d\n", len);
  std::fstream outfile;
  outfile.open("serialized.out", std::ofstream::out | std::ofstream::binary);
  outfile.write((const char *)buf, len);
  outfile.close();
#endif

#ifdef VERIFY
  // Get access to the root:
  auto ship = GetShip(builder.GetBufferPointer());

  // Get and test some scalar types from the FlatBuffer.
  assert(ship->name()->str() == "ALPHA028");

  // Get and test a field of the FlatBuffer's `struct`.
  auto location = ship->pos();
  assert(location);
  assert(location->latitude() == 63.04f);
  (void)location;
#endif


#ifdef MUTATE
  
  fstream infile;

  infile.open("serialized.out", std::ofstream::out | std::ofstream::binary);
  infile.read((const char *)buf, builder.GetSize());
  infile.close();


  printf("len %d\n", len);
#endif

#if 0
  // Get and test the `weapons` FlatBuffers's `vector`.
  std::string expected_weapon_names[] = { "Sword", "Axe" };
  short expected_weapon_damages[] = { 3, 5 };
  auto weps = monster->weapons();
  for (unsigned int i = 0; i < weps->size(); i++) {
    assert(weps->Get(i)->name()->str() == expected_weapon_names[i]);
    assert(weps->Get(i)->damage() == expected_weapon_damages[i]);
  }
  (void)expected_weapon_names;
  (void)expected_weapon_damages;

  // Get and test the `Equipment` union (`equipped` field).
  assert(monster->equipped_type() == Equipment_Weapon);
  auto equipped = static_cast<const Weapon *>(monster->equipped());
  assert(equipped->name()->str() == "Axe");
  assert(equipped->damage() == 5);
  (void)equipped;
#endif

  printf("The FlatBuffer was successfully created and verified!\n");
}
