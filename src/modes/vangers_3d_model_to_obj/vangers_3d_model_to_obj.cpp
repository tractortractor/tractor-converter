#include "vangers_3d_model_to_obj.hpp"



namespace tractor_converter{



double scale_from_map(
  const std::unordered_map<std::string, double> &scale_sizes,
  const boost::filesystem::path &to_lookup_path,
  const boost::filesystem::path &input_game_dir_root,
  const std::string &input_file_name_error,
  scale_from_map_type type,
  double default_scale)
{
  std::string to_lookup_str_name =
    boost::algorithm::to_lower_copy(to_lookup_path.stem().string());
  if(!scale_sizes.count(to_lookup_str_name))
  {
    boost::filesystem::path bogus_file_rel_root;
    if(type == scale_from_map_type::mechos)
    {
      bogus_file_rel_root =
        boost::filesystem::path(helpers::folder::resource) /
        boost::filesystem::path(helpers::folder::m3d) /
        boost::filesystem::path(helpers::folder::mechous) /
        boost::filesystem::path(to_lookup_str_name).append(
          helpers::ext::prm,
          boost::filesystem::path::codecvt());
    }
    else if(type == scale_from_map_type::non_mechos)
    {
      bogus_file_rel_root = boost::filesystem::path(helpers::file::game_lst);
    }

    boost::filesystem::path bogus_file =
      helpers::filepath_case_insensitive_part_get(input_game_dir_root,
                                                  bogus_file_rel_root);
    if(bogus_file.string().size())
    {
//    throw std::runtime_error(
//      input_file_name_error + " file " + bogus_file.string() +
//      " have unspecified scale_size for " +
//      to_lookup_path.string() + " file.");
      std::cout << '\n';
      std::cout << (input_file_name_error + " file " + bogus_file.string() +
        " have unspecified scale_size for " + to_lookup_path.string() +
        " file. Default scale " + std::to_string(default_scale) +
        " is used.") << '\n';
      return default_scale;
    }
    else
    {
      bogus_file = input_game_dir_root / bogus_file_rel_root;
//    throw std::runtime_error(
//      input_file_name_error + " file " + bogus_file.string() +
//      " doesn't exist but needed to provide scale_size for " +
//      to_lookup_path.string() + " file.");
      std::cout << '\n';
      std::cout << (input_file_name_error + " file " + bogus_file.string() +
        " doesn't exist but needed to provide scale_size for " +
        to_lookup_path.string() + " file. Default scale " +
        std::to_string(default_scale) + " is used.") << '\n';
      return default_scale;
    }
  }
  return scale_sizes.at(to_lookup_str_name);
}



void vangers_3d_model_to_obj_mode(
  const boost::program_options::variables_map options)
{
  try
  {
    const std::vector<std::string> options_to_check =
    {
      option::name::source_dir,
      option::name::output_dir,
    };
    helpers::check_options(options, options_to_check);



    boost::filesystem::path source_dir =
      helpers::get_directory(
        options[option::name::source_dir].as<std::string>(),
        option::name::source_dir);
    boost::filesystem::path output_dir =
      helpers::get_directory(
        options[option::name::output_dir].as<std::string>(),
        option::name::output_dir);
    int wavefront_float_precision =
      options[option::name::obj_float_precision].as<int>();
    double default_scale =
      options[option::name::default_scale].as<double>();
    std::string m3d_weapon_file =
      options[option::name::m3d_weapon_file].as<std::string>();
    boost::filesystem::path weapon_attachment_point_file =
      boost::filesystem::system_complete(
        options[option::name::weapon_attachment_point_file].as<std::string>());
    boost::filesystem::path ghost_wheel_file =
      boost::filesystem::system_complete(
        options[option::name::ghost_wheel_file].as<std::string>());
    boost::filesystem::path center_of_mass_file =
      boost::filesystem::system_complete(
        options[option::name::center_of_mass_file].as<std::string>());
    boost::filesystem::path wavefront_mtl =
      boost::filesystem::system_complete(
        options[option::name::wavefront_mtl].as<std::string>());

    helpers::bitflag<helpers::m3d_to_obj_flag> m3d_to_obj_flags;
    if(options[option::name::extract_bound_model].as<bool>())
    {
      m3d_to_obj_flags |=
        helpers::m3d_to_obj_flag::extract_bound_model;
    }
    if(options[option::name::extract_nonexistent_weapons].as<bool>())
    {
      m3d_to_obj_flags |=
        helpers::m3d_to_obj_flag::extract_nonexistent_weapons;
    }
    if(options[option::name::use_custom_volume_by_default].as<bool>())
    {
      m3d_to_obj_flags |=
        helpers::m3d_to_obj_flag::use_custom_volume_by_default;
    }
    if(options[option::name::use_custom_rcm_by_default].as<bool>())
    {
      m3d_to_obj_flags |=
        helpers::m3d_to_obj_flag::use_custom_rcm_by_default;
    }
    if(options[option::name::use_custom_J_by_default].as<bool>())
    {
      m3d_to_obj_flags |=
        helpers::m3d_to_obj_flag::use_custom_J_by_default;
    }



    if(wavefront_float_precision < volInt::min_float_precision)
    {
      throw std::runtime_error(
        option::name::obj_float_precision + " must be at least " +
        std::to_string(volInt::min_float_precision) + ".\n");
    }



    volInt::polyhedron weapon_attachment_point_model;
    volInt::polyhedron *weapon_attachment_point_model_ptr;
    try
    {
      // Getting weapon attachment point model to insert into weapon model.
      weapon_attachment_point_model =
        helpers::raw_obj_to_volInt_model(
          weapon_attachment_point_file,
          option::name::weapon_attachment_point_file,
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
        std::cout << "Generated weapon and mechos *.obj files " << 
          "should not be converted back to *.m3d since there will be " <<
          "no data about weapons' attachment points." << '\n';
        std::cout << '\n';
        weapon_attachment_point_model_ptr = nullptr;
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get weapon attachment point model: " <<
        e.what() << '\n';
      std::cout << "Generated weapon and mechos *.obj files " <<
        "should not be converted back to *.m3d since there will be " <<
        "no data about weapons' attachment points." << '\n';
      weapon_attachment_point_model_ptr = nullptr;
    }



    volInt::polyhedron ghost_wheel_model;
    volInt::polyhedron *ghost_wheel_model_ptr;
    try
    {
      // Getting ghost wheel model to insert
      // in place of wheels with no polygons.
      ghost_wheel_model =
        helpers::raw_obj_to_volInt_model(
          ghost_wheel_file,
          option::name::ghost_wheel_file,
          c3d::c3d_type::regular,
          c3d::color::string_to_id::wheel);
      ghost_wheel_model_ptr = &ghost_wheel_model;
      ghost_wheel_model.move_coord_system_to_center();
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get ghost wheel model: " <<
        e.what() << '\n';
      std::cout << "Generated mechos files will lose their ghost wheels " <<
        "if they had any." << '\n';
      ghost_wheel_model_ptr = nullptr;
    }



    volInt::polyhedron center_of_mass_model;
    volInt::polyhedron *center_of_mass_model_ptr;
    if(options[option::name::extract_center_of_mass].as<bool>())
    {
      try
      {
        // Getting center of mass model to mark extracted center of mass.
        center_of_mass_model =
          helpers::raw_obj_to_volInt_model(
            center_of_mass_file,
            option::name::center_of_mass_file,
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
          std::cout << "Center of mass will not be extracted." << '\n';
          std::cout << '\n';
          center_of_mass_model_ptr = nullptr;
        }
      }
      catch(std::exception &e)
      {
        std::cout << '\n';
        std::cout << "Failed to get center of mass model: " <<
          e.what() << '\n';
        std::cout << "Center of mass will not be extracted." << '\n';
        center_of_mass_model_ptr = nullptr;
      }
    }
    else
    {
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
         boost::algorithm::to_lower_copy(entry.path().filename().string()) ==
           helpers::file::game_lst)
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
          output_dir / rel_to_input_file.parent_path();
      }
    }

    // Getting list of paths with prm, m3d and a3d files.
    for(auto &&game_dir : vangers_game_dirs)
    {
      boost::filesystem::path resource_folder_name(helpers::folder::resource);
      boost::filesystem::path m3d_folder_name(helpers::folder::m3d);

      boost::filesystem::path input_resource =
        helpers::filepath_case_insensitive_part_get(game_dir.second.root.input,
                                                    resource_folder_name);
      boost::filesystem::path output_resource =
        game_dir.second.root.output / resource_folder_name;
      boost::filesystem::path input_resource_m3d =
        helpers::filepath_case_insensitive_part_get(input_resource,
                                                    m3d_folder_name);
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
        boost::filesystem::path rel_to_input_path_lowercase =
          boost::algorithm::to_lower_copy(rel_to_input_path.string());
        boost::filesystem::path abs_out_path =
          output_dir / rel_to_input_path_lowercase;
        boost::filesystem::path parent_abs_out_path =
          abs_out_path.parent_path();
        if(boost::filesystem::is_directory(entry.status()) &&
           !boost::filesystem::exists(abs_out_path))
        {
          boost::filesystem::copy_directory(entry.path(), abs_out_path);
        }
        else if(boost::filesystem::is_regular_file(entry.status()))
        {
          std::string out_parent_dir =
            boost::algorithm::to_lower_copy(
              entry.path().parent_path().filename().string());
          std::string out_file_name =
            boost::algorithm::to_lower_copy(entry.path().filename().string());
          std::string out_file_ext =
            boost::algorithm::to_lower_copy(entry.path().extension().string());

          if(helpers::vangers_3d_tree_folders.count(out_parent_dir))
          {
            if(out_parent_dir == helpers::folder::mechous &&
               out_file_ext ==   helpers::ext::prm)
            {
              game_dir.second.mechous_prm[out_file_name].input =
                entry.path();
              game_dir.second.mechous_prm[out_file_name].output =
                parent_abs_out_path;
            }
            else if(out_parent_dir == helpers::folder::mechous &&
                    out_file_ext ==   helpers::ext::m3d)
            {
              game_dir.second.mechous_m3d[out_file_name].input =
                entry.path();
              game_dir.second.mechous_m3d[out_file_name].output =
                parent_abs_out_path;
            }
            else if(out_parent_dir == helpers::folder::weapon &&
                    out_file_ext ==   helpers::ext::m3d)
            {
              game_dir.second.weapon_m3d[out_file_name].input =
                entry.path();
              game_dir.second.weapon_m3d[out_file_name].output =
                parent_abs_out_path;
            }
            else if(out_parent_dir == helpers::folder::animated &&
                    out_file_ext ==   helpers::ext::a3d)
            {
              game_dir.second.animated_a3d[out_file_name].input =
                entry.path();
              game_dir.second.animated_a3d[out_file_name].output =
                parent_abs_out_path;
            }
            else if(out_file_ext == helpers::ext::m3d)
            {
              game_dir.second.other_m3d[out_file_name].input =
                entry.path();
              game_dir.second.other_m3d[out_file_name].output =
                parent_abs_out_path;
            }
          }
        }
      }
    }



    // TEST
    /*
    for(const auto &game_dir : vangers_game_dirs)
    {
      std::cout << "game_dir.first: " << game_dir.first << '\n';
      std::cout << "game_dir.second.game_lst.input: " <<
        game_dir.second.game_lst.input << '\n';
      std::cout << "game_dir.second.game_lst.output: " <<
        game_dir.second.game_lst.output << '\n';

      for(const auto &prm_io_paths : game_dir.second.mechous_prm)
      {
        std::cout << "prm_io_paths.first: " << prm_io_paths.first << '\n';
        std::cout << "prm_io_paths.second.input: " <<
          prm_io_paths.second.input << '\n';
        std::cout << "prm_io_paths.second.output: " <<
          prm_io_paths.second.output << '\n';
      }


      for(const auto &m3d_io_paths : game_dir.second.weapon_m3d)
      {
        std::cout << "weapon_m3d.first: " << m3d_io_paths.first << '\n';
        std::cout << "weapon_m3d.second.input: " <<
          m3d_io_paths.second.input << '\n';
        std::cout << "weapon_m3d.second.output: " <<
          m3d_io_paths.second.output << '\n';
      }
      std::cout << "m3d_weapon_file: " << m3d_weapon_file << '\n';


      for(const auto &m3d_io_paths : game_dir.second.mechous_m3d)
      {
        std::cout << "mechous_m3d.first: " << m3d_io_paths.first << '\n';
        std::cout << "mechous_m3d.second.input: " <<
          m3d_io_paths.second.input << '\n';
        std::cout << "mechous_m3d.second.output: " <<
          m3d_io_paths.second.output << '\n';
      }


      for(const auto &m3d_io_paths : game_dir.second.animated_a3d)
      {
        std::cout << "animated_a3d.first: " << m3d_io_paths.first << '\n';
        std::cout << "animated_a3d.second.input: " <<
          m3d_io_paths.second.input << '\n';
        std::cout << "animated_a3d.second.output: " <<
          m3d_io_paths.second.output << '\n';
      }


      for(const auto &m3d_io_paths : game_dir.second.other_m3d)
      {
        std::cout << "other_m3d.first: " << m3d_io_paths.first << '\n';
        std::cout << "other_m3d.second.input: " <<
          m3d_io_paths.second.input << '\n';
        std::cout << "other_m3d.second.output: " <<
          m3d_io_paths.second.output << '\n';
      }
    }
    */



    // Converting files for each game directory.
    // It is assumed that each game directory have it's own
    // *.prm parameters and *.m3d weapons files.
    for(const auto &game_dir : vangers_game_dirs)
    {
      std::unordered_map<std::string, double> non_mechos_scale_sizes =
        helpers::read_scales_and_copy_game_lst(game_dir.second.game_lst.input,
                                               game_dir.second.game_lst.output,
                                               option::name::source_dir,
                                               option::name::output_dir);
      // TEST
      /*
      for(const auto &non_mechos_scale_size : non_mechos_scale_sizes)
      {
        std::cout << "non_mechos_scale_size.first: " <<
          non_mechos_scale_size.first << '\n';
        std::cout << "non_mechos_scale_size.second: " <<
          non_mechos_scale_size.second << '\n';
      }
      */


      std::unordered_map<std::string, double> mechos_scale_sizes;
      for(const auto &prm_io_paths : game_dir.second.mechous_prm)
      {
        std::string m3d_filename_lowercase =
          boost::algorithm::to_lower_copy(
            prm_io_paths.second.input.stem().string());
        mechos_scale_sizes[m3d_filename_lowercase] =
          helpers::read_scale_and_copy_prm(prm_io_paths.second.input,
                                           prm_io_paths.second.output,
                                           option::name::source_dir,
                                           option::name::output_dir);
      }
      // TEST
      /*
      for(const auto &mechos_scale_size : mechos_scale_sizes)
      {
        std::cout << "mechos_scale_size.first: " <<
          mechos_scale_size.first << '\n';
        std::cout << "mechos_scale_size.second: " <<
          mechos_scale_size.second << '\n';
      }
      */


      std::unordered_map<std::string, volInt::polyhedron> weapons_models;
      for(const auto &m3d_io_paths : game_dir.second.weapon_m3d)
      {
        double scale_size = scale_from_map(non_mechos_scale_sizes,
                                           m3d_io_paths.second.input,
                                           game_dir.second.root.input,
                                           option::name::source_dir,
                                           scale_from_map_type::non_mechos,
                                           default_scale);

        weapons_models[m3d_io_paths.second.input.stem().string()] =
          helpers::weapon_m3d_to_wavefront_objs(
            m3d_io_paths.second.input,
            m3d_io_paths.second.output,
            option::name::source_dir,
            option::name::output_dir,
            weapon_attachment_point_model_ptr,
            center_of_mass_model_ptr,
            scale_size,
            wavefront_float_precision,
            m3d_to_obj_flags);
      }

      volInt::polyhedron *mechos_weapon_model_ptr;
      if(weapons_models.count(m3d_weapon_file))
      {
        mechos_weapon_model_ptr = &weapons_models.at(m3d_weapon_file);
      }
      else
      {
        std::cout << '\n';
        std::cout << "Failed to get example weapon model: " <<
          m3d_weapon_file << '\n';
        std::cout << "Generated mechos *.obj files " <<
          "should not be converted back to *.m3d since there will be " <<
          "no data about weapons' positions." << '\n';
        std::cout << "You should change " << option::name::m3d_weapon_file <<
          " option to one of the following:" << '\n';
        for(const auto &weapons_model : weapons_models)
        {
          std::cout << weapons_model.first << '\n';
        }
        std::cout << '\n';
        mechos_weapon_model_ptr = NULL;
      }


      for(const auto &m3d_io_paths : game_dir.second.mechous_m3d)
      {
        double scale_size = scale_from_map(mechos_scale_sizes,
                                           m3d_io_paths.second.input,
                                           game_dir.second.root.input,
                                           option::name::source_dir,
                                           scale_from_map_type::mechos,
                                           default_scale);

        helpers::mechos_m3d_to_wavefront_objs(
          m3d_io_paths.second.input,
          m3d_io_paths.second.output,
          option::name::source_dir,
          option::name::output_dir,
          mechos_weapon_model_ptr,
          ghost_wheel_model_ptr,
          center_of_mass_model_ptr,
          scale_size,
          wavefront_float_precision,
          m3d_to_obj_flags);
      }


      for(const auto &a3d_io_paths : game_dir.second.animated_a3d)
      {
        double scale_size = scale_from_map(non_mechos_scale_sizes,
                                           a3d_io_paths.second.input,
                                           game_dir.second.root.input,
                                           option::name::source_dir,
                                           scale_from_map_type::non_mechos,
                                           default_scale);

        helpers::animated_a3d_to_wavefront_objs(
          a3d_io_paths.second.input,
          a3d_io_paths.second.output,
          option::name::source_dir,
          option::name::output_dir,
          center_of_mass_model_ptr,
          scale_size,
          wavefront_float_precision,
          m3d_to_obj_flags);
      }


      for(const auto &m3d_io_paths : game_dir.second.other_m3d)
      {
        double scale_size = scale_from_map(non_mechos_scale_sizes,
                                           m3d_io_paths.second.input,
                                           game_dir.second.root.input,
                                           option::name::source_dir,
                                           scale_from_map_type::non_mechos,
                                           default_scale);

        helpers::other_m3d_to_wavefront_objs(
          m3d_io_paths.second.input,
          m3d_io_paths.second.output,
          option::name::source_dir,
          option::name::output_dir,
          center_of_mass_model_ptr,
          scale_size,
          wavefront_float_precision,
          m3d_to_obj_flags);
      }

      boost::filesystem::path where_to_save_mtl;
      try
      {
        where_to_save_mtl =
          game_dir.second.root.output /
          boost::filesystem::path(wavefront_obj::mtl_filename);
        boost::filesystem::copy_file(
          wavefront_mtl,
          where_to_save_mtl,
          boost::filesystem::copy_option::overwrite_if_exists);
      }
      catch(std::exception &e)
      {
        std::cout << '\n';
        std::cout <<
          ("Failed to copy " + option::name::wavefront_mtl + " file " +
           wavefront_mtl.string() + " to " + where_to_save_mtl.string() +
           ". Exception caught: " + e.what()) << '\n';
      }
    }
  }
  catch(std::exception &)
  {
    std::cout << "vangers_3d_model_to_obj mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
