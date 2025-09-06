/**
 * @file model.cpp
 * @brief Implements the Model class for loading and rendering 3D models.
 *
 * This file contains the implementation of the Model class, which uses the Assimp
 * library to load complex 3D models from various file formats. It processes the
 * model's hierarchy, extracts mesh data (vertices, indices, textures), and
 * prepares it for rendering.
 */

#include "model.h"

/**
 * @brief Constructs a Model object.
 * @param path The file path to the 3D model.
 * @param gamma A flag indicating whether to apply gamma correction.
 */
Model::Model(std::string const &path, bool gamma) : gammaCorrection(gamma) {
    loadModel(path);
}

/**
 * @brief Renders all meshes in the model.
 * @param shader The shader program to use for drawing.
 */
void Model::Draw(Shader &shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

/**
 * @brief Loads a model from a file using Assimp.
 * @param path The file path of the model to load.
 */
void Model::loadModel(std::string const &path) {
    Assimp::Importer importer;
    // Read the model file with post-processing flags.
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
        aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // Check for loading errors.
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Store the directory of the model file for loading textures.
    directory = path.substr(0, path.find_last_of('/'));
    // Start processing the nodes recursively from the root node.
    processNode(scene->mRootNode, scene);
}

/**
 * @brief Recursively processes each node in the Assimp scene hierarchy.
 * @param node The current node to process.
 * @param scene The Assimp scene object.
 */
void Model::processNode(aiNode *node, const aiScene *scene) {
    // Process all meshes in the current node.
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // Recursively process all child nodes.
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

/**
 * @brief Processes an individual mesh, extracting vertex data, indices, and materials.
 * @param mesh The Assimp mesh object to process.
 * @param scene The Assimp scene object.
 * @return A Mesh object containing the processed data.
 */
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Extract vertex data.
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        // Position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // Texture Coordinates, Tangents, and Bitangents
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // Bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Extract indices from faces.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process materials and load textures.
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. Diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. Specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. Normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. Height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // Return a new Mesh object created from the extracted data.
    return Mesh(vertices, indices, textures);
}

/**
 * @brief Loads textures of a given type from a material.
 * @param mat The Assimp material.
 * @param type The type of texture to load (e.g., diffuse, specular).
 * @param typeName A string name for the texture type.
 * @return A vector of Texture objects.
 */
std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Prevent loading the same texture multiple times.
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            // If the texture hasn't been loaded yet, load it.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // Add to loaded textures cache.
        }
    }
    return textures;
}
