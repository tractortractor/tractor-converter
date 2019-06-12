#include "obj_to_vangers_3d_model.hpp"



namespace tractor_converter{



void obj_to_vangers_3d_model_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      "source_dir",
      "output_dir"
    };
    helpers::check_options(options, options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(options["source_dir"].as<std::string>(),
                             "source_dir");
    boost::filesystem::path output_dir =
      helpers::get_directory(options["output_dir"].as<std::string>(),
                             "output_dir");
    boost::filesystem::path weapon_attachment_point_file =
      boost::filesystem::system_complete(
        options["weapon_attachment_point_file"].as<std::string>());
    boost::filesystem::path center_of_mass_file =
      boost::filesystem::system_complete(
        options["center_of_mass_file"].as<std::string>());
    std::string c3d_default_material_str =
      options["c3d_default_material"].as<std::string>();
    double scale_cap = options["3d_scale_cap"].as<double>();

    helpers::bitflag<helpers::obj_to_m3d_flag> obj_to_m3d_flags;
    if(options["center_model"].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::center_model;
    }
    if(options["recalculate_vertex_normals"].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::recalculate_vertex_normals;
    }
    if(options["generate_bound_models"].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::generate_bound_models;
    }

    unsigned int c3d_default_material_id;
    try
    {
      c3d_default_material_id =
        c3d::color::ids.by<c3d::color::name>().at(c3d_default_material_str);
      if(c3d_default_material_id >= c3d::color::string_to_id::max_colors_ids)
      {
        throw std::runtime_error(
          "Material is valid but should not be used as default.");
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Invalid \"c3d_default_material\" option " <<
        c3d_default_material_str << '\n';
      std::cout << e.what() << '\n';
      std::cout << "Using " TRACTOR_CONVERTER_DEFAULT_C3D_DEFAULT_MATERIAL
        " as default." << '\n';

      std::cout << "Valid materials:" << '\n';
      for(const auto &material : c3d::color::ids.by<c3d::color::id>())
      {
        if(material.first < c3d::color::string_to_id::max_colors_ids)
        {
          std::cout << material.second << '\n';
        }
      }
      std::cout << '\n';

      c3d_default_material_id =
        c3d::color::ids.by<c3d::color::name>().at(
          TRACTOR_CONVERTER_DEFAULT_C3D_DEFAULT_MATERIAL);
    }


    volInt::polyhedron weapon_attachment_point_model;
    volInt::polyhedron *weapon_attachment_point_model_ptr;
    try
    {
      // Getting weapon attachment point model to get positions of the weapons.
      weapon_attachment_point_model = helpers::raw_obj_to_volInt_model(
        weapon_attachment_point_file,
        "weapon_attachment_point_file",
        c3d::c3d_type::regular,
        c3d::color::string_to_id::attachment_point);
      weapon_attachment_point_model_ptr = &weapon_attachment_point_model;

      if(!weapon_attachment_point_model_ptr->find_ref_points())
      {
        std::cout << '\n';
        std::cout << "Failed to get weapon attachment point model " <<
          weapon_attachment_point_file.string() << '\n';
        std::cout << '\n';
        std::cout << "Can't find 3 reference vertices." << '\n';
        std::cout << "Generated *.m3d mechos and weapon files " <<
          "will have incorrect attachment point data." << '\n';
        std::cout << '\n';
        weapon_attachment_point_model_ptr = nullptr;
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get weapon attachment point model: " <<
        e.what() << '\n';
      std::cout << "Generated *.m3d mechos and weapon files may have " <<
        "incorrect weapon attachment point data." << '\n';
      weapon_attachment_point_model_ptr = nullptr;
    }



    volInt::polyhedron center_of_mass_model;
    volInt::polyhedron *center_of_mass_model_ptr;
    try
    {
      // Getting center of mass model
      // to generate inertia tensor with custom center of mass.
      center_of_mass_model =
        helpers::raw_obj_to_volInt_model(
          center_of_mass_file,
          "center_of_mass_file",
          c3d::c3d_type::regular,
          c3d::color::string_to_id::center_of_mass);
      center_of_mass_model_ptr = &center_of_mass_model;

      if(!center_of_mass_model_ptr->find_ref_points())
      {
        std::cout << '\n';
        std::cout << "Failed to get center of mass model " <<
          center_of_mass_file.string() << '\n';
        std::cout << '\n';
        std::cout << "Can't find 3 reference vertices." << '\n';
        std::cout << "Generated *.m3d and *.a3d files " <<
          "will not have custom center of mass." << '\n';
        std::cout << '\n';
        center_of_mass_model_ptr = nullptr;
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get center of mass model: " << e.what() << '\n';
      std::cout << "Center of mass marker will not be used " <<
        "for generation of *.m3d/*.a3d files." << '\n';
      center_of_mass_model_ptr = nullptr;
    }



    // Getting list of game directories.
    // Game.lst file must be present for each game directory.
    std::unordered_map<std::string, helpers::vangers_3d_paths_game_dir>
      vangers_game_dirs;
    for(const auto &entry :
        boost::filesystem::recursive_directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(entry.status()) &&
         entry.path().filename() == "game.lst")
      {
        boost::filesystem::path rel_to_input_file =
          entry.path().lexically_relative(source_dir);
        boost::filesystem::path rel_to_input_parent =
          entry.path().parent_path().lexically_relative(source_dir);
        vangers_game_dirs[rel_to_input_parent.string()].root.input =
          entry.path().parent_path();
        vangers_game_dirs[rel_to_input_parent.string()].root.output =
          output_dir / rel_to_input_parent;
        vangers_game_dirs[rel_to_input_parent.string()].game_lst.input =
          entry.path();
        vangers_game_dirs[rel_to_input_parent.string()].game_lst.output =
          output_dir / rel_to_input_file;
      }
    }

    // Getting list of paths with prm, m3d and a3d files.
    for(auto &&game_dir : vangers_game_dirs)
    {
      boost::filesystem::path resource_folder_name("resource");
      boost::filesystem::path m3d_folder_name("m3d");
      boost::filesystem::path input_resource =
        game_dir.second.root.input / resource_folder_name;
      boost::filesystem::path output_resource =
        game_dir.second.root.output / resource_folder_name;
      boost::filesystem::path input_resource_m3d =
        input_resource / m3d_folder_name;
      boost::filesystem::path output_resource_m3d =
        output_resource / m3d_folder_name;

      if(!boost::filesystem::exists(output_resource))
      {
        boost::filesystem::copy_directory(input_resource,
                                          output_resource);
      }
      if(!boost::filesystem::exists(output_resource_m3d))
      {
        boost::filesystem::copy_directory(input_resource_m3d,
                                          output_resource_m3d);
      }


      for(const auto &entry :
          boost::filesystem::recursive_directory_iterator(input_resource_m3d))
      {
        boost::filesystem::path rel_to_input_path =
          entry.path().lexically_relative(source_dir);
        boost::filesystem::path abs_out_path = output_dir / rel_to_input_path;
        boost::filesystem::path parent_abs_out_path =
          abs_out_path.parent_path();
        if(boost::filesystem::is_directory(entry.status()))
        {
          std::string parent_dir =
            entry.path().parent_path().filename().string();
          std::string dir_name = entry.path().filename().string();

          if(std::find(helpers::vangers_3d_tree_folders.begin(),
                       helpers::vangers_3d_tree_folders.end(),
                       parent_dir) !=
             helpers::vangers_3d_tree_folders.end())
          {
            if(parent_dir == "mechous")
            {
              game_dir.second.mechous_m3d[dir_name].input =
                entry.path();
              game_dir.second.mechous_m3d[dir_name].output =
                parent_abs_out_path;
            }
            else if(parent_dir == "weapon")
            {
              game_dir.second.weapon_m3d[dir_name].input =
                entry.path();
              game_dir.second.weapon_m3d[dir_name].output =
                parent_abs_out_path;
            }
            else if(parent_dir == "animated")
            {
              game_dir.second.animated_a3d[dir_name].input =
                entry.path();
              game_dir.second.animated_a3d[dir_name].output =
                parent_abs_out_path;
            }
            else
            {
              game_dir.second.other_m3d[dir_name].input =
                entry.path();
              game_dir.second.other_m3d[dir_name].output =
                parent_abs_out_path;
            }
          }
          else if(!boost::filesystem::exists(abs_out_path))
          {
            boost::filesystem::copy_directory(entry.path(), abs_out_path);
          }
        }
      }
    }



    // Converting files for each game directory.
    // It is assumed that each game directory
    // have it's own *.prm parameters and *.m3d weapons files.
    for(const auto &game_dir : vangers_game_dirs)
    {
      std::unordered_map<std::string, double> non_mechos_scale_sizes;
      std::unordered_map<std::string, double> *non_mechos_scale_sizes_ptr =
        &non_mechos_scale_sizes;


      std::unordered_map<std::string, volInt::polyhedron> weapons_models;
      weapons_models.reserve(game_dir.second.weapon_m3d.size());
      for(const auto &m3d_io_paths : game_dir.second.weapon_m3d)
      {
        try
        {
          weapons_models[m3d_io_paths.second.input.stem().string()] =
            helpers::weapon_wavefront_objs_to_m3d(
              m3d_io_paths.second.input,
              m3d_io_paths.second.output,
              "source_dir",
              "output_dir",
              weapon_attachment_point_model_ptr,
              center_of_mass_model_ptr,
              c3d_default_material_id,
              scale_cap,
              obj_to_m3d_flags,
              non_mechos_scale_sizes_ptr);
          // TEST
//        std::cout << '\n' << "weapon m3d" << '\n';
//        std::cout << "input: " <<
//          m3d_io_paths.second.input.string() << '\n';
//        std::cout << "output: " <<
//          m3d_io_paths.second.output.string() << '\n';
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load weapon files " <<
            "from source_dir *.obj directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as *.m3d file in output_dir directory " <<
            m3d_io_paths.second.output.string() << '\n';
          std::cout << e.what() << '\n';
        }
      }



      double max_weapons_radius =
        helpers::get_weapons_bound_sphere_radius(weapons_models);

      volInt::polyhedron *mechos_weapon_model_ptr = nullptr;



      for(const auto &m3d_io_paths : game_dir.second.mechous_m3d)
      {
        try
        {
          helpers::mechos_wavefront_objs_to_m3d(
            m3d_io_paths.second.input,
            m3d_io_paths.second.output,
            "source_dir",
            "output_dir",
            mechos_weapon_model_ptr,
            weapon_attachment_point_model_ptr,
            center_of_mass_model_ptr,
            max_weapons_radius,
            c3d_default_material_id,
            scale_cap,
            obj_to_m3d_flags);
          // TEST
//        std::cout << '\n' << "mechos m3d" << '\n';
//        std::cout << "input: " <<
//          m3d_io_paths.second.input.string() << '\n';
//        std::cout << "output: " <<
//          m3d_io_paths.second.output.string() << '\n';
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load mechos files " <<
            "from source_dir *.obj directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as *.m3d file in output_dir directory " <<
            m3d_io_paths.second.output.string() << '\n';
          std::cout << e.what() << '\n';
        }
      }


      for(const auto &a3d_io_paths : game_dir.second.animated_a3d)
      {
        try
        {
          helpers::animated_wavefront_objs_to_a3d(
            a3d_io_paths.second.input,
            a3d_io_paths.second.output,
            "source_dir",
            "output_dir",
            center_of_mass_model_ptr,
            c3d_default_material_id,
            scale_cap,
            obj_to_m3d_flags,
            non_mechos_scale_sizes_ptr);
          // TEST
//        std::cout << '\n' << "animated a3d" << '\n';
//        std::cout << "input: " <<
//          a3d_io_paths.second.input.string() << '\n';
//        std::cout << "output: " <<
//          a3d_io_paths.second.output.string() << '\n';
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load animated files " <<
            "from source_dir *.obj directory " <<
            a3d_io_paths.second.input.string() <<
            " and save them as *.a3d file in output_dir directory " <<
            a3d_io_paths.second.output.string() << '\n';
          std::cout << e.what() << '\n';
        }
      }


      for(const auto &m3d_io_paths : game_dir.second.other_m3d)
      {
        try
        {
          helpers::other_wavefront_objs_to_m3d(
            m3d_io_paths.second.input,
            m3d_io_paths.second.output,
            "source_dir",
            "output_dir",
            center_of_mass_model_ptr,
            c3d_default_material_id,
            scale_cap,
            obj_to_m3d_flags,
            non_mechos_scale_sizes_ptr);
          // TEST
//        std::cout << '\n' << "other m3d" << '\n';
//        std::cout << "input: " <<
//          m3d_io_paths.second.input.string() << '\n';
//        std::cout << "output: " <<
//          m3d_io_paths.second.output.string() << '\n';
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load files " <<
            "from source_dir *.obj directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as *.m3d file in output_dir directory " <<
            m3d_io_paths.second.output.string() << '\n';
          std::cout << e.what() << '\n';
        }
      }



      helpers::create_game_lst(game_dir.second.game_lst.input,
                               game_dir.second.game_lst.output,
                               "source_dir",
                               "output_dir",
                               non_mechos_scale_sizes_ptr);
    }
  }
  catch(std::exception &)
  {
    std::cout << "obj_to_vangers_3d_model mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
