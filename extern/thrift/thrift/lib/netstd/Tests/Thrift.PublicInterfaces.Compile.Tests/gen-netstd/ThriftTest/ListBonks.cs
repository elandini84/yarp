/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Thrift;
using Thrift.Collections;
using System.ServiceModel;
using System.Runtime.Serialization;

using Thrift.Protocol;
using Thrift.Protocol.Entities;
using Thrift.Protocol.Utilities;
using Thrift.Transport;
using Thrift.Transport.Client;
using Thrift.Transport.Server;
using Thrift.Processor;


#pragma warning disable IDE0079  // remove unnecessary pragmas
#pragma warning disable IDE1006  // parts of the code use IDL spelling

namespace ThriftTest
{

[DataContract(Namespace="")]
public partial class ListBonks : TBase
{
  private List<global::ThriftTest.Bonk> _bonk;

  [DataMember(Order = 0)]
  public List<global::ThriftTest.Bonk> Bonk
  {
    get
    {
      return _bonk;
    }
    set
    {
      __isset.bonk = true;
      this._bonk = value;
    }
  }


  [DataMember(Order = 1)]
  public Isset __isset;
  [DataContract]
  public struct Isset
  {
    [DataMember]
    public bool bonk;
  }

  #region XmlSerializer support

  public bool ShouldSerializeBonk()
  {
    return __isset.bonk;
  }

  #endregion XmlSerializer support

  public ListBonks()
  {
  }

  public ListBonks DeepCopy()
  {
    var tmp173 = new ListBonks();
    if((Bonk != null) && __isset.bonk)
    {
      tmp173.Bonk = this.Bonk.DeepCopy();
    }
    tmp173.__isset.bonk = this.__isset.bonk;
    return tmp173;
  }

  public async global::System.Threading.Tasks.Task ReadAsync(TProtocol iprot, CancellationToken cancellationToken)
  {
    iprot.IncrementRecursionDepth();
    try
    {
      TField field;
      await iprot.ReadStructBeginAsync(cancellationToken);
      while (true)
      {
        field = await iprot.ReadFieldBeginAsync(cancellationToken);
        if (field.Type == TType.Stop)
        {
          break;
        }

        switch (field.ID)
        {
          case 1:
            if (field.Type == TType.List)
            {
              {
                TList _list174 = await iprot.ReadListBeginAsync(cancellationToken);
                Bonk = new List<global::ThriftTest.Bonk>(_list174.Count);
                for(int _i175 = 0; _i175 < _list174.Count; ++_i175)
                {
                  global::ThriftTest.Bonk _elem176;
                  _elem176 = new global::ThriftTest.Bonk();
                  await _elem176.ReadAsync(iprot, cancellationToken);
                  Bonk.Add(_elem176);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          default: 
            await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            break;
        }

        await iprot.ReadFieldEndAsync(cancellationToken);
      }

      await iprot.ReadStructEndAsync(cancellationToken);
    }
    finally
    {
      iprot.DecrementRecursionDepth();
    }
  }

  public async global::System.Threading.Tasks.Task WriteAsync(TProtocol oprot, CancellationToken cancellationToken)
  {
    oprot.IncrementRecursionDepth();
    try
    {
      var struc = new TStruct("ListBonks");
      await oprot.WriteStructBeginAsync(struc, cancellationToken);
      var field = new TField();
      if((Bonk != null) && __isset.bonk)
      {
        field.Name = "bonk";
        field.Type = TType.List;
        field.ID = 1;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, Bonk.Count), cancellationToken);
          foreach (global::ThriftTest.Bonk _iter177 in Bonk)
          {
            await _iter177.WriteAsync(oprot, cancellationToken);
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      await oprot.WriteFieldStopAsync(cancellationToken);
      await oprot.WriteStructEndAsync(cancellationToken);
    }
    finally
    {
      oprot.DecrementRecursionDepth();
    }
  }

  public override bool Equals(object that)
  {
    if (!(that is ListBonks other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.bonk == other.__isset.bonk) && ((!__isset.bonk) || (TCollections.Equals(Bonk, other.Bonk))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if((Bonk != null) && __isset.bonk)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Bonk);
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var sb = new StringBuilder("ListBonks(");
    int tmp178 = 0;
    if((Bonk != null) && __isset.bonk)
    {
      if(0 < tmp178++) { sb.Append(", "); }
      sb.Append("Bonk: ");
      Bonk.ToString(sb);
    }
    sb.Append(')');
    return sb.ToString();
  }
}

}