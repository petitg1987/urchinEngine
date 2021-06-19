bl_info = {
    "name": "Export Urchin Engine (.urchinMesh, .urchinAnim)",
    "version": (1, 0, 0),
    "blender": (2, 80, 0),
    "api": 31847,
    "location": "File > Export",
    "description": "Export Urchin Engine (.urchinMesh, .urchinAnim)",
    "category": "Import-Export"}

import bpy, struct, math, os, time, sys, mathutils, enum
from enum import Enum


# ---------------------------------------------------------------------------
# MATH UTILS
# ---------------------------------------------------------------------------
def point_by_matrix(p, m):
    return [p[0] * m[0][0] + p[1] * m[1][0] + p[2] * m[2][0] + m[3][0],
            p[0] * m[0][1] + p[1] * m[1][1] + p[2] * m[2][1] + m[3][1],
            p[0] * m[0][2] + p[1] * m[1][2] + p[2] * m[2][2] + m[3][2]]


def matrix_invert(m):
    det = (m.col[0][0] * (m.col[1][1] * m.col[2][2] - m.col[2][1] * m.col[1][2])
           - m.col[1][0] * (m.col[0][1] * m.col[2][2] - m.col[2][1] * m.col[0][2])
           + m.col[2][0] * (m.col[0][1] * m.col[1][2] - m.col[1][1] * m.col[0][2]))
    if det == 0.0: return None
    det = 1.0 / det

    r = [[
        det * (m.col[1][1] * m.col[2][2] - m.col[2][1] * m.col[1][2]),
        - det * (m.col[0][1] * m.col[2][2] - m.col[2][1] * m.col[0][2]),
        det * (m.col[0][1] * m.col[1][2] - m.col[1][1] * m.col[0][2]),
        0.0,
    ], [
        - det * (m.col[1][0] * m.col[2][2] - m.col[2][0] * m.col[1][2]),
        det * (m.col[0][0] * m.col[2][2] - m.col[2][0] * m.col[0][2]),
        - det * (m.col[0][0] * m.col[1][2] - m.col[1][0] * m.col[0][2]),
        0.0
    ], [
        det * (m.col[1][0] * m.col[2][1] - m.col[2][0] * m.col[1][1]),
        - det * (m.col[0][0] * m.col[2][1] - m.col[2][0] * m.col[0][1]),
        det * (m.col[0][0] * m.col[1][1] - m.col[1][0] * m.col[0][1]),
        0.0,
    ]]
    r.append([
        - (m.col[3][0] * r[0][0] + m.col[3][1] * r[1][0] + m.col[3][2] * r[2][0]),
        - (m.col[3][0] * r[0][1] + m.col[3][1] * r[1][1] + m.col[3][2] * r[2][1]),
        - (m.col[3][0] * r[0][2] + m.col[3][1] * r[1][2] + m.col[3][2] * r[2][2]),
        1.0,
    ])
    return r

# ---------------------------------------------------------------------------
# GLOBAL VARIABLES
# ---------------------------------------------------------------------------
bones = {}
reporter = None

# ---------------------------------------------------------------------------
# MESH DATA OBJECTS
# ---------------------------------------------------------------------------
class Material:
    name = ""

    def __init__(self, material_file_name):
        self.name = material_file_name

    def to_urchin_mesh(self):
        return self.name


class Mesh:
    name = ""
    sub_meshes = []
    next_sub_mesh_id = 0

    def __init__(self, name):
        self.name = name
        self.sub_meshes = []
        self.next_sub_mesh_id = 0

    def to_urchin_mesh(self):
        mesh_number = 0
        buf = ""
        for sub_mesh in self.sub_meshes:
            buf = buf + "mesh {\n"
            mesh_number += 1
            buf = buf + sub_mesh.to_urchin_mesh()
            buf = buf + "}\n\n"

        return buf


class SubMesh:
    def __init__(self, mesh, material):
        self.material = material
        self.vertices = []
        self.faces = []
        self.weights = []

        self.next_vertex_id = 0
        self.next_linked_vertices_group_id = 0
        self.next_weight_id = 0

        self.mesh = mesh
        self.name = mesh.name
        self.id = mesh.next_sub_mesh_id
        mesh.next_sub_mesh_id += 1
        mesh.sub_meshes.append(self)

    def bind_to_mesh(self, mesh):
        self.mesh.sub_meshes.remove(self)
        self.mesh = mesh
        self.id = mesh.next_sub_mesh_id
        mesh.next_sub_mesh_id += 1
        mesh.sub_meshes.append(self)

    def generate_weights(self):
        self.weights = []
        self.next_weight_id = 0
        for vert in self.vertices:
            vert.generate_weights()

    def report_double_faces(self):
        for face in self.faces:
            for face2 in self.faces:
                if not face == face2:
                    if (not face.vertex1 == face2.vertex1) and (not face.vertex1 == face2.vertex2) and (not face.vertex1 == face2.vertex3):
                        return
                    if (not face.vertex2 == face2.vertex1) and (not face.vertex2 == face2.vertex2) and (not face.vertex2 == face2.vertex3):
                        return
                    if (not face.vertex3 == face2.vertex1) and (not face.vertex3 == face2.vertex2) and (not face.vertex3 == face2.vertex3):
                        return
                    reporter.report({'WARNING'}, "Double face detected: " + str(face) + " " + str(face2))

    def to_urchin_mesh(self):
        self.generate_weights()
        self.report_double_faces()

        buf = "\tmaterial \"%s\"\n\n" % (self.material.to_urchin_mesh())

        # vertices
        buf = buf + "\tnumVerts %i\n" % (len(self.vertices))
        vnumber = 0
        for vert in self.vertices:
            buf = buf + "\tvert %i %s\n" % (vnumber, vert.to_urchin_mesh())
            vnumber += 1

        # faces
        buf = buf + "\n\tnumTris %i\n" % (len(self.faces))
        facenumber = 0
        for face in self.faces:
            buf = buf + "\ttri %i %s\n" % (facenumber, face.to_urchin_mesh())
            facenumber += 1

        # weights
        buf = buf + "\n\tnumWeights %i\n" % (len(self.weights))
        weightnumber = 0
        for weight in self.weights:
            buf = buf + "\tweight %i %s\n" % (weightnumber, weight.to_urchin_mesh())
            weightnumber += 1

        return buf


class CloneReason(Enum):
    NOT_CLONED = 1
    FLAT_FACE = 2
    DIFFERENT_TEXTURE_COORD = 3


class Vertex:
    def __init__(self, sub_mesh, coord, cloned_from=None, clone_reason=CloneReason.NOT_CLONED):
        self.id = sub_mesh.next_vertex_id
        self.coord = coord
        self.texture_coord = None
        self.weights = []
        self.weight = None
        self.first_weight_index = 0
        self.clones = []
        self.sub_mesh = sub_mesh
        self.cloned_from = cloned_from

        if self.cloned_from is not None:
            self.influences = cloned_from.influences
            cloned_from.clones.append(self)
        else:
            self.influences = []

        if clone_reason == CloneReason.DIFFERENT_TEXTURE_COORD:
            self.linked_vertices_group_id = self.cloned_from.linked_vertices_group_id
        else:
            self.linked_vertices_group_id = sub_mesh.next_linked_vertices_group_id
            sub_mesh.next_linked_vertices_group_id += 1

        sub_mesh.next_vertex_id += 1
        sub_mesh.vertices.append(self)

    def generate_weights(self):
        self.first_weight_index = self.sub_mesh.next_weight_id
        for influence in self.influences:
            self.sub_mesh.next_weight_id += 1
            new_weight = Weight(influence.bone, influence.weight, self, self.coord[0], self.coord[1], self.coord[2])
            self.sub_mesh.weights.append(new_weight)
            self.weights.append(new_weight)

    def to_urchin_mesh(self):
        buf = "%i " % self.linked_vertices_group_id
        if self.texture_coord:
            buf = buf + self.texture_coord.to_urchin_mesh()
        else:
            buf = buf + "( 0.0 0.0 )"
        buf = buf + " ( %i %i )" % (self.first_weight_index, len(self.influences))
        return buf


class TextureCoordinate:
    def __init__(self, u, v):
        self.u = u
        self.v = v

    def to_urchin_mesh(self):
        buf = "( %f %f )" % (self.u, self.v)
        return buf


class Weight:
    def __init__(self, bone, weight, vertex, x, y, z):
        self.bone = bone
        self.weight = weight
        self.vertex = vertex
        inv_bone_matrix = matrix_invert(self.bone.matrix)
        self.x, self.y, self.z = point_by_matrix((x, y, z), inv_bone_matrix)

    def to_urchin_mesh(self):
        buf = "%i %f ( %f %f %f )" % (self.bone.id, self.weight, self.x, self.y, self.z)
        return buf


class Influence:
    def __init__(self, bone, weight):
        self.bone = bone
        self.weight = weight


class Face:
    def __init__(self, sub_mesh, vertex1, vertex2, vertex3):
        self.vertex1 = vertex1
        self.vertex2 = vertex2
        self.vertex3 = vertex3

        self.sub_mesh = sub_mesh
        sub_mesh.faces.append(self)

    def to_urchin_mesh(self):
        buf = "%i %i %i" % (self.vertex1.id, self.vertex3.id, self.vertex2.id)
        return buf


class Skeleton:
    def __init__(self):
        self.bones = []
        self.next_bone_id = 0

    def to_urchin_mesh(self, num_sub_meshes):
        buf = "numJoints %i\n" % self.next_bone_id
        buf = buf + "numMeshes %i\n\n" % num_sub_meshes
        buf = buf + "joints {\n"
        for bone in self.bones:
            buf = buf + bone.to_urchin_mesh()
        buf = buf + "}\n\n"
        return buf


class Bone:
    def __init__(self, skeleton, parent, name, mat):
        self.parent = parent
        self.name = name
        self.children = []

        self.matrix = mat
        if parent:
            parent.children.append(self)

        self.skeleton = skeleton
        self.id = skeleton.next_bone_id
        skeleton.next_bone_id += 1
        skeleton.bones.append(self)

        bones[name] = self

    def to_urchin_mesh(self):
        buf = "\t\"%s\"\t" % self.name
        parent_index = -1
        if self.parent:
            parent_index = self.parent.id
        buf = buf + "%i " % parent_index

        pos1, pos2, pos3 = self.matrix.col[3][0], self.matrix.col[3][1], self.matrix.col[3][2]
        buf = buf + "( %f %f %f ) " % (pos1, pos2, pos3)

        bquat = self.matrix.to_quaternion()
        bquat.normalize()
        qx = bquat.x
        qy = bquat.y
        qz = bquat.z
        if bquat.w > 0:
            qx = -qx
            qy = -qy
            qz = -qz
        buf = buf + "( %f %f %f )" % (qx, qy, qz)
        buf = buf + "\n"
        return buf


# ---------------------------------------------------------------------------
# MESH ANIMATION DATA OBJECTS
# ---------------------------------------------------------------------------
class UrchinAnimation:
    def __init__(self, skeleton):
        self.frame_data = []  # frame_data[bone_id] holds the data for each frame
        self.bounds = []
        self.base_frame = []
        self.skeleton = skeleton
        self.bone_flags = []
        self.bone_frame_data_index = []  # stores the frame_dataIndex for each bone in the skeleton
        self.frame_rate = 24
        self.num_frames = 0
        self.num_animated_components = 0
        for _ in self.skeleton.bones:
            self.frame_data.append([])
            self.base_frame.append([])
            self.bone_flags.append(0)
            self.bone_frame_data_index.append(0)

    def to_urchin_anim(self):
        current_frame_data_index = 0
        for bone in self.skeleton.bones:
            if len(self.frame_data[bone.id]) > 0:
                if len(self.frame_data[bone.id]) > self.num_frames:
                    self.num_frames = len(self.frame_data[bone.id])

                (x, y, z), (qw, qx, qy, qz) = self.frame_data[bone.id][0]
                self.base_frame[bone.id] = (x, y, z, qx, qy, qz)
                self.bone_frame_data_index[bone.id] = current_frame_data_index
                self.bone_flags[bone.id] = 63
                current_frame_data_index += 6
                self.num_animated_components = current_frame_data_index
            else:
                rot = bone.matrix.to_quaternion()
                rot.normalize()
                qx = rot.x
                qy = rot.y
                qz = rot.z
                if rot.w > 0:
                    qx = -qx
                    qy = -qy
                    qz = -qz
                self.base_frame.col[bone.id] = (bone.matrix.col[3][0], bone.matrix.col[3][1], bone.matrix.col[3][2], qx, qy, qz)

        buf = "numFrames %i\n" % self.num_frames
        buf = buf + "numJoints %i\n" % (len(self.skeleton.bones))
        buf = buf + "frameRate %i\n" % self.frame_rate
        buf = buf + "numAnimatedComponents %i\n\n" % self.num_animated_components
        buf = buf + "hierarchy {\n"

        for bone in self.skeleton.bones:
            parent_index = -1
            flags = self.bone_flags[bone.id]
            frame_data_index = self.bone_frame_data_index[bone.id]
            if bone.parent:
                parent_index = bone.parent.id
            buf = buf + "\t\"%s\"\t%i %i %i" % (bone.name, parent_index, flags, frame_data_index)
            if bone.parent:
                buf = buf + " " + bone.parent.name
            buf = buf + "\n"
        buf = buf + "}\n\n"

        buf = buf + "bounds {\n"
        for b in self.bounds:
            buf = buf + "\t( %f %f %f ) ( %f %f %f )\n" % b
        buf = buf + "}\n\n"

        buf = buf + "base_frame {\n"
        for b in self.base_frame:
            buf = buf + "\t( %f %f %f ) ( %f %f %f )\n" % b
        buf = buf + "}\n\n"

        for f in range(0, self.num_frames):
            buf = buf + "frame %i {\n" % f
            for b in self.skeleton.bones:
                if len(self.frame_data[b.id]) > 0:
                    (x, y, z), (qw, qx, qy, qz) = self.frame_data[b.id][f]
                    if qw > 0:
                        qx, qy, qz = -qx, -qy, -qz
                    buf = buf + "\t%f %f %f %f %f %f\n" % (x, y, z, qx, qy, qz)
            buf = buf + "}\n\n"

        return buf

    def add_key_for_bone(self, bone_id, loc, rot):
        self.frame_data[bone_id].append([loc, rot])
        return


# ---------------------------------------------------------------------------
# EXPORT MAIN
# ---------------------------------------------------------------------------
class UrchinSettings:
    def __init__(self, save_path, export_mode):
        self.save_path = save_path
        self.export_mode = export_mode


def treat_bone(skeleton, b, w_matrix, parent=None):
    print("[INFO] Processing bone: " + b.name)
    if parent and not b.parent.name == parent.name:
        return  # only catch direct children
    mat = mathutils.Matrix(w_matrix) @ mathutils.Matrix(b.matrix_local)
    new_bone = Bone(skeleton, parent, b.name, mat)
    if b.children:
        for child in b.children:
            treat_bone(skeleton, child, w_matrix, new_bone)


def get_min_max(list_of_points):
    if len(list_of_points) == 0:
        return [0, 0, 0], [0, 0, 0]
    min = [list_of_points[0][0], list_of_points[0][1], list_of_points[0][2]]
    max = [list_of_points[0][0], list_of_points[0][1], list_of_points[0][2]]
    for i in range(1, len(list_of_points)):
        if len(list_of_points[i]) == 3:
            if list_of_points[i][0] > max[0]: max[0] = list_of_points[i][0]
            if list_of_points[i][1] > max[1]: max[1] = list_of_points[i][1]
            if list_of_points[i][2] > max[2]: max[2] = list_of_points[i][2]
            if list_of_points[i][0] < min[0]: min[0] = list_of_points[i][0]
            if list_of_points[i][1] < min[1]: min[1] = list_of_points[i][1]
            if list_of_points[i][2] < min[2]: min[2] = list_of_points[i][2]
    return min, max


def objects_to_export():
    objects_list = []
    for obj in bpy.context.selected_objects:
        if (obj is not None) and (obj.type == 'MESH') and (len(obj.data.vertices.values()) > 0):
            objects_list.append(obj)

    if len(objects_list) == 0:
        for obj in bpy.context.visible_objects:
            if (obj is not None) and (obj.type == 'MESH') and (len(obj.data.vertices.values()) > 0):
                objects_list.append(obj)

    return objects_list


def generate_bounding_box(urchin_animation, frame_range):
    print("[INFO] Generating animation bounding box")
    scene = bpy.context.scene
    for i in range(frame_range[0], frame_range[1] + 1):
        corners = []
        scene.frame_set(i)

        for obj in objects_to_export():
            if obj.data.polygons:
                bbox = obj.bound_box

                matrix = [[1.0, 0.0, 0.0, 0.0],
                          [0.0, 1.0, 0.0, 0.0],
                          [0.0, 1.0, 1.0, 0.0],
                          [0.0, 0.0, 0.0, 1.0],
                          ]
                for v in bbox:
                    corners.append(point_by_matrix(v, matrix))
        (min, max) = get_min_max(corners)
        urchin_animation.bounds.append((min[0], min[1], min[2], max[0], max[1], max[2]))


def save_urchin(settings):
    print("[INFO] Exporting selected objects...")
    bpy.ops.object.mode_set(mode='OBJECT')

    # COMMON EXPORT
    bpy.context.scene.frame_set(bpy.context.scene.frame_start)
    curr_armature = 0
    skeleton = Skeleton()
    for obj in objects_to_export():
        curr_armature = obj.find_armature()
        if curr_armature:
            w_matrix = curr_armature.matrix_world
            print("[INFO] Processing armature: " + curr_armature.name)
            for b in curr_armature.data.bones:
                if not b.parent:  # only treat root bones
                    treat_bone(skeleton, b, w_matrix)
        else:
            reporter.report({'ERROR'}, "Armature not found on object: " + obj.name)
        break

    # MESH EXPORT
    meshes = []
    for obj in objects_to_export():
        # Get the evaluate object => object where modifiers (triangulation...) are applied.
        # More details: https://docs.blender.org/api/blender2.8/bpy.types.Depsgraph.html
        deps_graph = bpy.context.evaluated_depsgraph_get()
        object_eval = obj.evaluated_get(deps_graph)
        modified_mesh = object_eval.to_mesh()

        mesh = Mesh(modified_mesh.name)
        print("[INFO] Processing mesh: " + modified_mesh.name)
        meshes.append(mesh)

        num_tris = 0
        num_weights = 0
        for f in modified_mesh.polygons:
            num_tris += len(f.vertices) - 2
        for v in modified_mesh.vertices:
            num_weights += len(v.groups)

        w_matrix = obj.matrix_world
        verts = modified_mesh.vertices

        faces = []
        for f in modified_mesh.polygons:
            faces.append(f)

        create_vertex_a = 0  # normal vertex
        create_vertex_b = 0  # cloned because flat face
        create_vertex_c = 0  # cloned because different texture coord

        while faces:
            material_index = faces[0].material_index
            material = Material(modified_mesh.materials[0].name)

            sub_mesh = SubMesh(mesh, material)
            vertices = {}
            for face in faces[:]:
                if len(face.vertices) < 3:
                    faces.remove(face)
                elif face.vertices[0] == face.vertices[1]:
                    faces.remove(face)
                elif face.vertices[0] == face.vertices[2]:
                    faces.remove(face)
                elif face.vertices[1] == face.vertices[2]:
                    faces.remove(face)
                elif face.material_index == material_index:
                    faces.remove(face)
                    face_vertices = []
                    for i in range(len(face.vertices)):
                        vertex = False
                        if face.vertices[i] in vertices:
                            vertex = vertices[face.vertices[i]]

                        if not vertex:  # found unique vertex, add to list
                            coord = point_by_matrix(verts[face.vertices[i]].co, w_matrix)

                            vertex = Vertex(sub_mesh, coord)
                            if face.use_smooth:  # smooth face can share vertex, not flat face
                                vertices[face.vertices[i]] = vertex
                            create_vertex_a += 1

                            influences = []
                            for j in range(len(modified_mesh.vertices[face.vertices[i]].groups)):
                                inf = [obj.vertex_groups[modified_mesh.vertices[face.vertices[i]].groups[j].group].name, modified_mesh.vertices[face.vertices[i]].groups[j].weight]
                                influences.append(inf)

                            if not influences:
                                reporter.report({'WARNING'}, "There is a vertex without bone attachment in mesh: " + mesh.name)
                            sum = 0.0
                            for bone_name, weight in influences:
                                sum += weight

                            for bone_name, weight in influences:
                                if sum != 0:
                                    try:
                                        vertex.influences.append(Influence(bones[bone_name], weight / sum))
                                    except:
                                        continue
                                else:  # we have a vertex that is probably not skinned. export anyway
                                    try:
                                        vertex.influences.append(Influence(bones[bone_name], weight))
                                    except:
                                        continue

                        elif not face.use_smooth:
                            vertex = Vertex(sub_mesh, vertex.coord, vertex, CloneReason.FLAT_FACE)
                            create_vertex_b += 1

                        active_uv_layer = modified_mesh.uv_layers.active
                        if active_uv_layer is not None and len(active_uv_layer.data) > 0:
                            uv_layer_data = active_uv_layer.data
                            uv = [uv_layer_data[face.loop_start + i].uv[0], uv_layer_data[face.loop_start + i].uv[1]]
                            uv[1] = 1.0 - uv[1]
                            if not vertex.texture_coord:
                                vertex.texture_coord = TextureCoordinate(*uv)
                            elif (vertex.texture_coord.u != uv[0]) or (vertex.texture_coord.v != uv[1]):
                                for clone in vertex.clones:
                                    if (clone.texture_coord.u == uv[0]) and (clone.texture_coord.v == uv[1]):
                                        vertex = clone
                                        break
                                else:  # clone vertex because different texture coord
                                    vertex = Vertex(sub_mesh, vertex.coord, vertex, CloneReason.DIFFERENT_TEXTURE_COORD)
                                    vertex.texture_coord = TextureCoordinate(*uv)
                                    create_vertex_c += 1

                        face_vertices.append(vertex)

                    for i in range(1, len(face.vertices) - 1):  # split faces with more than 3 vertices
                        Face(sub_mesh, face_vertices[0], face_vertices[i], face_vertices[i + 1])
                else:
                    reporter.report({'ERROR'}, "Found face with invalid material on object: " + obj.name)
        print("[INFO] Created vertices: A " + str(create_vertex_a) + ", B " + str(create_vertex_b) + ", C " + str(create_vertex_c))

    if settings.export_mode == "mesh & anim" or settings.export_mode == "mesh only":
        urchin_mesh_filename = settings.save_path + ".urchinMesh"

        if len(meshes) > 1:  # save all submeshes in the first mesh
            for mesh in range(1, len(meshes)):
                for sub_mesh in meshes[mesh].sub_meshes:
                    sub_mesh.bind_to_mesh(meshes[0])

        try:
            file = open(urchin_mesh_filename, 'w')
            buffer = skeleton.to_urchin_mesh(len(meshes[0].sub_meshes))
            buffer = buffer + meshes[0].to_urchin_mesh()
            file.write(buffer)
            file.close()
            print("[INFO] Saved mesh to " + urchin_mesh_filename)
        except IOError:
            reporter.report({'ERROR'}, "IOError to write in: " + urchin_mesh_filename)

    # ANIMATION EXPORT
    if settings.export_mode == "mesh & anim" or settings.export_mode == "anim only":
        animations = {}
        range_start = 0
        range_end = 0
        if curr_armature.animation_data and curr_armature.animation_data.action:
            armature_action = curr_armature.animation_data.action
            animation = animations[armature_action.name] = UrchinAnimation(skeleton)

            frame_min, frame_max = armature_action.frame_range
            print("[INFO] Frame start: " + str(frame_min))
            print("[INFO] Frame end: " + str(frame_max))
            range_start = int(frame_min)
            range_end = int(frame_max)

            current_time = range_start
            while current_time <= range_end:
                bpy.context.scene.frame_set(current_time)
                pose = curr_armature.pose

                for bone_name in curr_armature.data.bones.keys():
                    pose_bone_mat = mathutils.Matrix(pose.bones[bone_name].matrix)

                    try:
                        bone = bones[bone_name]
                    except:
                        reporter.report({'ERROR'}, "Found a posebone animating a bone that is not part of the exported armature: " + bone_name)
                        continue
                    if bone.parent:
                        parent_pose_mat = mathutils.Matrix(pose.bones[bone.parent.name].matrix)
                        parent_pose_mat.invert()
                        pose_bone_mat = parent_pose_mat @ pose_bone_mat
                    else:
                        pose_bone_mat = curr_armature.matrix_world @ pose_bone_mat
                    loc = [pose_bone_mat.col[3][0],
                           pose_bone_mat.col[3][1],
                           pose_bone_mat.col[3][2],
                           ]
                    rot = pose_bone_mat.to_quaternion()
                    rot.normalize()
                    rot = [rot.w, rot.x, rot.y, rot.z]

                    animation.add_key_for_bone(bone.id, loc, rot)
                current_time += 1
        else:
            reporter.report({'ERROR'}, "Animation data missing.")

        urchin_anim_filename = settings.save_path + ".urchinAnim"
        if len(animations) > 0:
            anim = animations.popitem()[1]
            try:
                file = open(urchin_anim_filename, 'w')
                generate_bounding_box(anim, [range_start, range_end])
                buffer = anim.to_urchin_anim()
                file.write(buffer)
                file.close()
                print("[INFO] Saved anim to " + urchin_anim_filename)
            except IOError:
                reporter.report({'ERROR'}, "IOError to write in: " + urchin_anim_filename)
        else:
            reporter.report({'ERROR'}, "No urchinAnim file generated.")


# ---------------------------------------------------------------------------
# UI
# ---------------------------------------------------------------------------
class ExportUrchin(bpy.types.Operator):
    """Export Urchin Engine (.urchinMesh .urchinAnim)"""
    bl_idname = "export.urchin"
    bl_label = 'Export Urchin Engine'

    export_modes = [("mesh & anim", "Mesh & Anim", "Export mesh and anim"),
                    ("anim only", "Anim only", "Export anim only"),
                    ("mesh only", "Mesh only", "Export mesh only")]

    filepath: bpy.props.StringProperty(subtype='FILE_PATH', name="File Path", description="File path for exporting", maxlen=1024, default="")
    export_mode: bpy.props.EnumProperty(name="Exports", items=export_modes, description="Choose export mode", default='mesh only')

    def execute(self, context):
        global reporter
        reporter = self
        settings = UrchinSettings(save_path=self.properties.filepath, export_mode=self.properties.export_mode)
        save_urchin(settings)
        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {"RUNNING_MODAL"}


def menu_func(self, context):
    default_path = os.path.splitext(bpy.data.filepath)[0]
    self.layout.operator(ExportUrchin.bl_idname, text="Urchin Engine (.urchinMesh .urchinAnim)", icon='BLENDER').filepath = default_path


classes = (
    ExportUrchin,
)


def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_file_export.append(menu_func)


def unregister():
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func)


if __name__ == "__main__":
    register()
