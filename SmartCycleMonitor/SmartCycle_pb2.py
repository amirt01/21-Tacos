# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: SmartCycle.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x10SmartCycle.proto\"\xbf\x02\n\x0cServerStatus\x12\r\n\x05speed\x18\x01 \x01(\x02\x12\x0f\n\x07\x63\x61\x64\x65nce\x18\x02 \x01(\x02\x12\x13\n\x0btarget_gear\x18\x03 \x01(\x05\x12\x14\n\x0c\x63urrent_gear\x18\x04 \x01(\x05\x12\"\n\x05state\x18\x05 \x01(\x0e\x32\x13.ServerStatus.State\x12\x32\n\x0fup_shift_button\x18\x06 \x01(\x0e\x32\x19.ServerStatus.ButtonState\x12\x34\n\x11\x64own_shift_button\x18\x07 \x01(\x0e\x32\x19.ServerStatus.ButtonState\",\n\x05State\x12\n\n\x06\x42IKING\x10\x00\x12\x0b\n\x07STOPPED\x10\x01\x12\n\n\x06\x41SLEEP\x10\x02\"(\n\x0b\x42uttonState\x12\x0c\n\x08RELEASED\x10\x00\x12\x0b\n\x07PRESSED\x10\x01\x62\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'SmartCycle_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _SERVERSTATUS._serialized_start=21
  _SERVERSTATUS._serialized_end=340
  _SERVERSTATUS_STATE._serialized_start=254
  _SERVERSTATUS_STATE._serialized_end=298
  _SERVERSTATUS_BUTTONSTATE._serialized_start=300
  _SERVERSTATUS_BUTTONSTATE._serialized_end=340
# @@protoc_insertion_point(module_scope)
