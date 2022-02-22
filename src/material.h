#pragma once
#ifndef CATA_SRC_MATERIAL_H
#define CATA_SRC_MATERIAL_H

#include <cstddef>
#include <iosfwd>
#include <map>
#include <new>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "fire.h"
#include "optional.h"
#include "translations.h"
#include "type_id.h"

class material_type;

enum class damage_type : int;
class JsonObject;

using mat_burn_products = std::vector<std::pair<itype_id, float>>;
using material_list = std::vector<material_type>;
using material_id_list = std::vector<material_id>;

struct fuel_explosion_data {
    int explosion_chance_hot = 0;
    int explosion_chance_cold = 0;
    float explosion_factor = 0.0f;
    bool fiery_explosion = false;
    float fuel_size_factor = 0.0f;

    bool is_empty();

    bool was_loaded = false;
    void load( const JsonObject &jsobj );
    void deserialize( const JsonObject &jo );
};

struct fuel_data {
    public:
        /** Energy of the fuel (kilojoules per charge) */
        float energy = 0.0f;
        fuel_explosion_data explosion_data;
        std::string pump_terrain = "t_null";
        bool is_perpetual_fuel = false;

        bool was_loaded = false;
        void load( const JsonObject &jsobj );
        void deserialize( const JsonObject &jo );
};

class material_type
{
    public:
        material_id id;
        std::vector<std::pair<material_id, mod_id>> src;
        bool was_loaded = false;

    private:
        translation _name;
        cata::optional<itype_id> _salvaged_into; // this material turns into this item when salvaged
        itype_id _repaired_with = itype_id( "null" ); // this material can be repaired with this item
        float _bash_resist = 0.0f;                         // negative integers means susceptibility
        float _cut_resist = 0.0f;
        float _acid_resist = 0.0f;
        float _elec_resist = 0.0f;
        float _fire_resist = 0.0f;
        float _bullet_resist = 0.0f;
        int _chip_resist = 0;                         // Resistance to physical damage of the item itself
        int _density = 1;                             // relative to "powder", which is 1
        // How resistant this material is to wind as a percentage - 0 to 100
        cata::optional<int> _wind_resist;
        float _specific_heat_liquid = 4.186f;
        float _specific_heat_solid = 2.108f;
        float _latent_heat = 334.0f;
        float _freeze_point = 0; // Celsius
        bool _edible = false;
        bool _rotting = false;
        bool _soft = false;
        bool _reinforces = false;

        translation _bash_dmg_verb;
        translation _cut_dmg_verb;
        std::vector<translation> _dmg_adj;

        std::map<vitamin_id, double> _vitamins;

        std::vector<mat_burn_data> _burn_data;

        fuel_data fuel;

        //Burn products defined in JSON as "burn_products": [ [ "X", float efficiency ], [ "Y", float efficiency ] ]
        mat_burn_products _burn_products;

    public:
        material_type();

        void load( const JsonObject &jsobj, const std::string &src );
        void check() const;

        material_id ident() const;
        std::string name() const;
        /**
         * @returns An empty optional if this material can not be
         * salvaged into any items (e.g. for powder, liquids).
         * Or a valid id of the item type that this can be salvaged
         * into (e.g. clothes made of material leather can be salvaged
         * into lather patches).
         */
        cata::optional<itype_id> salvaged_into() const;
        itype_id repaired_with() const;
        float bash_resist() const;
        float cut_resist() const;
        float bullet_resist() const;
        std::string bash_dmg_verb() const;
        std::string cut_dmg_verb() const;
        std::string dmg_adj( int damage ) const;
        float acid_resist() const;
        float elec_resist() const;
        float fire_resist() const;
        int chip_resist() const;
        float specific_heat_liquid() const;
        float specific_heat_solid() const;
        float latent_heat() const;
        float freeze_point() const;
        int density() const;
        cata::optional<int> wind_resist() const;
        bool edible() const;
        bool rotting() const;
        bool soft() const;
        bool reinforces() const;

        double vitamin( const vitamin_id &id ) const {
            const auto iter = _vitamins.find( id );
            return iter != _vitamins.end() ? iter->second : 0;
        }

        fuel_data get_fuel_data() const;

        const mat_burn_data &burn_data( size_t intensity ) const;
        const mat_burn_products &burn_products() const;
};

namespace materials
{

void load( const JsonObject &jo, const std::string &src );
void check();
void reset();

material_list get_all();
std::set<material_id> get_rotting();

} // namespace materials

#endif // CATA_SRC_MATERIAL_H
