#include "obj_to_vangers_3d_model.hpp"



namespace tractor_converter{



void obj_to_vangers_3d_model_mode(
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
    boost::filesystem::path weapon_attachment_point_file =
      boost::filesystem::system_complete(
        options[option::name::weapon_attachment_point_file].as<std::string>());
    boost::filesystem::path center_of_mass_file =
      boost::filesystem::system_complete(
        options[option::name::center_of_mass_file].as<std::string>());
    std::string default_c3d_material_str =
      options[option::name::default_c3d_material].as<std::string>();
    double scale_cap = options[option::name::scale_cap].as<double>();
    double max_smooth_angle =
      helpers::get_angle_option(options,
                                option::name::max_smooth_angle,
                                error_handling::throw_exception);
    std::size_t gen_bound_layers_num =
      options[option::name::gen_bound_layers_num].as<std::size_t>();
    double gen_bound_area_threshold =
      options[option::name::gen_bound_area_threshold].as<double>();
    if(gen_bound_area_threshold > option::max::gen_bound_area_threshold)
    {
      std::cout << option::name::gen_bound_area_threshold << " " <<
        gen_bound_area_threshold << " is more than expected max " <<
        option::max::gen_bound_area_threshold << '\n';
      std::cout << option::name::gen_bound_area_threshold << " is set to " <<
        option::max::gen_bound_area_threshold << '\n';
      gen_bound_area_threshold = option::max::gen_bound_area_threshold;
    }

    helpers::bitflag<helpers::obj_to_m3d_flag> obj_to_m3d_flags;
    if(options[option::name::center_model].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::center_model;
    }
    if(options[option::name::recalculate_vertex_normals].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::recalculate_vertex_normals;
    }
    if(options[option::name::gen_bound_models].as<bool>())
    {
      obj_to_m3d_flags |=
        helpers::obj_to_m3d_flag::generate_bound_models;
    }

    unsigned int default_c3d_material_id;
    try
    {
      default_c3d_material_id =
        c3d::color::ids.by<c3d::color::name>().at(default_c3d_material_str);
      if(default_c3d_material_id >= c3d::color::string_to_id::max_colors_ids)
      {
        throw std::runtime_error(
          "Material is valid but should not be used as default.");
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Invalid \"default_c3d_material\" option " <<
        default_c3d_material_str << '\n';
      std::cout << e.what() << '\n';
      std::cout << "Using " << option::default_val::default_c3d_material <<
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

      default_c3d_material_id =
        c3d::color::ids.by<c3d::color::name>().at(
          option::default_val::default_c3d_material);
    }


    volInt::polyhedron weapon_attachment_point_model;
    volInt::polyhedron *weapon_attachment_point_model_ptr;
    try
    {
      // Getting weapon attachment point model to get positions of weapons.
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
        std::cout << "Couldn't find 3 reference vertices." << '\n';
        std::cout << "Generated " << ext::readable::m3d <<
          " mechos and weapon files will have "
          "incorrect attachment point data." << '\n';
        std::cout << '\n';
        weapon_attachment_point_model_ptr = nullptr;
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get weapon attachment point model: " <<
        e.what() << '\n';
      std::cout << "Generated " << ext::readable::m3d <<
        " mechos and weapon files will have " <<
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
        std::cout << "Couldn't find 3 reference vertices." << '\n';
        std::cout << "Generated " << ext::readable::m3d_and_a3d <<
          " files will not have custom center of mass." << '\n';
        std::cout << '\n';
        center_of_mass_model_ptr = nullptr;
      }
    }
    catch(std::exception &e)
    {
      std::cout << '\n';
      std::cout << "Failed to get center of mass model: " << e.what() << '\n';
      std::cout << "Center of mass marker will not be used " <<
        "for generation of " + ext::readable::m3d_and_a3d + " files." << '\n';
      center_of_mass_model_ptr = nullptr;
    }



    // Getting list of game directories.
    // game.lst file must be present for each game directory.
    std::unordered_map<std::string, helpers::vangers_3d_paths_game_dir>
      vangers_game_dirs;
    for(const auto &entry :
        boost::filesystem::recursive_directory_iterator(source_dir))
    {
      if(boost::filesystem::is_regular_file(entry.status()) &&
         entry.path().filename() == file::game_lst)
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

    // Getting list of paths with *.prm, *.m3d and *.a3d files.
    for(auto &&game_dir : vangers_game_dirs)
    {
      boost::filesystem::path resource_folder_name(folder::resource);
      boost::filesystem::path m3d_folder_name(folder::m3d);
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

          if(helpers::vangers_3d_tree_folders.count(parent_dir))
          {
            if(parent_dir == folder::mechous)
            {
              game_dir.second.mechous_m3d[dir_name].input =
                entry.path();
              game_dir.second.mechous_m3d[dir_name].output =
                parent_abs_out_path;
            }
            else if(parent_dir == folder::weapon)
            {
              game_dir.second.weapon_m3d[dir_name].input =
                entry.path();
              game_dir.second.weapon_m3d[dir_name].output =
                parent_abs_out_path;
            }
            else if(parent_dir == folder::animated)
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
        else
        {
          if(entry.path().filename().string() == file::default_prm)
          {
            boost::filesystem::copy_file(
              entry.path(),
              abs_out_path,
              boost::filesystem::copy_option::overwrite_if_exists);
          }
        }
      }
    }



    // Converting files for each game directory.
    // It is assumed that each game directory
    // has its own *.prm parameters and *.m3d weapon files.
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
              option::name::source_dir,
              option::name::output_dir,
              weapon_attachment_point_model_ptr,
              center_of_mass_model_ptr,
              default_c3d_material_id,
              scale_cap,
              max_smooth_angle,
              gen_bound_layers_num,
              gen_bound_area_threshold,
              obj_to_m3d_flags,
              non_mechos_scale_sizes_ptr);
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load weapon files " <<
            "from " << option::name::source_dir << " " <<
            ext::readable::obj << " directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as " << ext::readable::m3d <<
            " file in " << option::name::output_dir << " directory " <<
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
            option::name::source_dir,
            option::name::output_dir,
            mechos_weapon_model_ptr,
            weapon_attachment_point_model_ptr,
            center_of_mass_model_ptr,
            max_weapons_radius,
            default_c3d_material_id,
            scale_cap,
            max_smooth_angle,
            gen_bound_layers_num,
            gen_bound_area_threshold,
            obj_to_m3d_flags);
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load mechos files " <<
            "from " << option::name::source_dir << " " <<
            ext::readable::obj << " directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as " << ext::readable::m3d <<
            " file in " << option::name::output_dir << " directory " <<
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
            option::name::source_dir,
            option::name::output_dir,
            center_of_mass_model_ptr,
            default_c3d_material_id,
            scale_cap,
            max_smooth_angle,
            obj_to_m3d_flags,
            non_mechos_scale_sizes_ptr);
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load animated files " <<
            "from " << option::name::source_dir << " " <<
            ext::readable::obj << " directory " <<
            a3d_io_paths.second.input.string() <<
            " and save them as " << ext::readable::a3d <<
            " file in " << option::name::output_dir << " directory " <<
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
            option::name::source_dir,
            option::name::output_dir,
            center_of_mass_model_ptr,
            default_c3d_material_id,
            scale_cap,
            max_smooth_angle,
            gen_bound_layers_num,
            gen_bound_area_threshold,
            obj_to_m3d_flags,
            non_mechos_scale_sizes_ptr);
        }
        catch(std::exception &e)
        {
          std::cout << '\n';
          std::cout << "Failed to load files " <<
            "from " << option::name::source_dir << " " <<
            ext::readable::obj << " directory " <<
            m3d_io_paths.second.input.string() <<
            " and save them as " << ext::readable::m3d <<
            " file in " << option::name::output_dir << " directory " <<
            m3d_io_paths.second.output.string() << '\n';
          std::cout << e.what() << '\n';
        }
      }



      helpers::create_game_lst(game_dir.second.game_lst.input,
                               game_dir.second.game_lst.output,
                               option::name::source_dir,
                               option::name::output_dir,
                               non_mechos_scale_sizes_ptr);
    }
  }
  catch(std::exception &)
  {
    std::cout << mode::name::obj_to_vangers_3d_model << " mode failed" << '\n';
    throw;
  }
}



} // namespace tractor_converter
