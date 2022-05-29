// int                        Response::handle_dir(std::string target_file, std::string body, struct stat &status)
// {
//     if (this->regular_path != "/")  
//         return handle_dir_response(target_file);
//     else if (this->regular_path == "/")
//     {
//         search_dir_in_locations("/");
//         return handle_special_dir(target_file, status, body);
//     }
//     else
//     {
//         if (!this->my_servers[_index].get_locations()[this->pos].get_autoindex())
//         {
//             std::cout << "handle_dir1" << std::endl;
//             error_handling("403 Forbidden");
//             return 0;
//         }
//         if (!check_dir(this->abs_path))
//         {
//             std::cout << "handle_dir2" << std::endl;
//             error_handling("500 Internal Server Error");
//             return 0;
//         }
//         std::cout << "handle_dir3" << std::endl;
//         this->my_Res << "Content-Type: text/html\r\n";
//         this->my_Res << "Content-Length: " << dir_list.length() << "\r\n";
//         body = "";
//     }
//     return 1;
// }

// int                       Response::handle_special_dir(std::string target_file, struct stat &status, std::string & body)
// {

//     if (this->check_index_of_dir())
//     {
//         if (stat(this->abs_path.c_str(), &status) < 0)
//         {
//             error_handling("403 Forbidden");
//             return 0;
//         }
//         else
//         {
//             this->my_Res << "Content-Type: text/html\r\n";
//             this->my_Res << "Content-Length: " << status.st_size << "\r\n\r\n";
//             get_body(this->abs_path);
//             this->set_hello(this->my_Res.str());
//             this->total_size = this->my_Res.str().size();
//             return 0;
//         }        
//     }
//     else
//     {
//         if (this->pos  == -1)
//         {
//             if (!this->my_servers[_index].get_autoindex())
//             {
//                 error_handling("403 Forbidden");
//                 return 0;
//             }
//         }
//         else
//         {
//             if (!this->my_servers[_index].get_locations()[this->pos].get_autoindex())
//             {
//                 error_handling("403 Forbidden");
//                 return 0;
//             }
//         }
//         this->my_Res << "Content-Type: text/html\r\n";
//         this->my_Res << "Content-Length: " << dir_list.length() << "\r\n\r\n";
//         this->my_Res << this->dir_list;
//         this->set_hello(this->my_Res.str());
//         this->total_size = this->my_Res.str().size();
//     }
//     return 0;
// }

// int                        Response::handle_dir_response(std::string target_file)
// {
//     this->Edit_path();
//     if (std::count(this->abs_path.begin(), this->abs_path.end(), '/') > 1)
//     {
//         if (this->check_index_of_dir())
//             return 1;
//         if (!this->my_servers[_index].get_locations()[this->pos].get_autoindex())
//         {
//             error_handling("403 Forbidden");
//             return 0;
//         }
//         if (!check_dir(this->abs_path))
//         {
//             error_handling("500 Internal Server Error");
//             return 0;
//         }
//         this->my_Res << "Content-Type: text/html\r\n";
//         this->my_Res << "Content-Length: " << dir_list.length() << "\r\n\r\n";
//         this->my_Res << this->dir_list;
//         this->set_hello(this->my_Res.str());
//         this->total_size = this->my_Res.str().size();
//     }
//     else
//     {
//         error_handling("404 not found");
//         this->set_hello(this->my_Res.str());
//         this->total_size = this->my_Res.str().size();
//     }
//     return 0;
// }

// int                       Response::check_index_of_dir()
// {
//     std::string         body;
//     struct stat         status;

//     std::cout << "check_index_of_dir1" << std::endl;
//     for(int i=0;i<this->my_servers[_index].get_locations()[this->pos].get_index().size();i++)
//     {
//         body = this->abs_path + this->my_servers[_index].get_locations()[this->pos].get_index()[i];
        
//         if (stat(body.c_str(), &status) >= 0)
//         {
//             this->abs_path = body;
//             this->handle_file(status);
//             return 1;
//         }
//     }
//     return 0;
// }