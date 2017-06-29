using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using Biu.Transmission;
using Biu.Transmission.Management;
using Biu.Transmission.Ready;

namespace Biu.Diagnostics
{
    public sealed class NetDebugLogRecorder : ILogRecorder, ITransmissionHop
    {
        private SecurityProvider _securityProvider;
        private Ashley _ashley;
        private AppServer _server;
        private BiuIoActionTable _actionTable = new BiuIoActionTable();

        private AshleyHeader _headerLogRecord;


        public IPEndPoint DebugN1EndPoint { get; set; }


        #region ILogRecordConverter

        private bool _isAvailable = true;
        public bool IsAvailable
        {
            get
            {
                return _isAvailable && _server.IsRunning;
            }

            set
            {
                _isAvailable = value;
            }
        }

        public ILogRecordConverter RecordConverter { get; set; }

        

        public bool Initialize()
        {
            try
            {
                TransmissionManager.StartUpWSA();

                _securityProvider = new SecurityProvider();
                _securityProvider.NewItem();

                _ashley = Ashley.CreateEntityProtocol();
                _server = new AppServer();

                BiuIoManager.Instance.NewHandler(NetDebugLogTransmissionManager.Handler_DebugN1, NetDebugLogTransmissionManager.ID_N1, 0, 0);
                //BiuIoManager.Instance.NewHandler(NetDebugLogTransmissionManager.Handler_DebugN1_Data, NetDebugLogTransmissionManager.ID_N1, 0, 0);

                _actionTable.Add(NetDebugLogTransmissionManager.Handler_DebugN1)
                    .AddRow((ulong)BiuAction.Accept, IOOperation.Read)
                    .AddRow((ulong)BiuAction.PublicKey, DebugN1_PublicKey);

                return true;
            }
            catch
            {
                throw;
            }
        }



        public void Uninitialize()
        {
            try
            {
                BiuIoManager.Instance.RemoveHandler(NetDebugLogTransmissionManager.Handler_DebugN1);

                _server = null;

                _ashley.Dispose();
                _ashley = null;

                _securityProvider.Dispose();
                _securityProvider = null;

                TransmissionManager.CleanUpWSA();
            }
            catch
            {
                throw;
            }
        }

        public bool Open(object arg)
        {
            try
            {
                _headerLogRecord = Ashley.CreateHeader(AshleyVersion.V1_1, NetDebugLogTransmissionManager.ID_N1,
                    (ulong)BiuNetDebugAction.LogRecord, AshleyFlag.Security);
                _headerLogRecord.Security.Alg = AshleySecurityAlg.P4S;


                _server.CC = new ConnectionCreation()
                {
                    Trait = SocketTrait.Normal,
                    Type = SocketType.IPv4TCP,
                    Protocol = _ashley,
                    Handler = BiuIoManager.Instance.Handlers[NetDebugLogTransmissionManager.Handler_DebugN1].Handler,
                    EndPoint = this.DebugN1EndPoint,
                    SecurityProvider = _securityProvider
                };

                TransmissionRouter.Instance.AddHop(this);
                
                return _server.Create(true);
            }
            catch
            {
                throw;
            }
        }

        public void Close()
        {
            try
            {
                _server.Close(true, true);

                TransmissionRouter.Instance.RemoveHop(this);

                _headerLogRecord = null;
            }
            catch
            {
                throw;
            }
        }

        public void Record(LogRecord logRecord)
        {
            if (null == logRecord)
            {
                return;
            }

            try
            {
                var converter = this.RecordConverter as ILogRecordConverter<Tso.NetLogRecord>;
                if (null == converter)
                {
                    return;
                }

                IBiuSerializable biuser = converter.ConvertTo(logRecord);
                if (null == biuser)
                {
                    return;
                }
                
                _server.ServerContext.ForEachPeer(peerContext =>
                {
                    var peer = _server.ServerContext.Server.GetPeerByKey(peerContext.Key);
                    peer.Send(biuser, _headerLogRecord);
                });
            }
            catch
            {
                throw;
            }
        }

        #endregion // ILogRecordConverter


        #region ITransmissionHop

        public string HopName
        {
            get
            {
                return "BiuNetLogRecorder";
            }
        }

        public void EnterHop()
        {

            var command = BiuIoManager.Instance.Handlers[NetDebugLogTransmissionManager.Handler_DebugN1].Command;
            command.Accepted += DebugN1_Accepted;
            command.Connected += DebugN1_Connected;
            command.CloseProactive += DebugN1_CloseProactive;
            command.ClosePassive += DebugN1_ClosePassive;
            command.WriteDone += DebugN1_WriteDone;
            command.Timeout += DebugN1_Timeout;
            command.File += DebugN1_File;
            command.FileNext += DebugN1_FileNext;
            command.ConnectionReset += DebugN1_ConnectionReset;
            command.ConnectRefused += DebugN1_ConnectRefused;
            command.HostUnavailable += DebugN1_HostUnavailable;
            command.NetNameUnavailable += DebugN1_NetNameUnavailable;
            command.NetUnavailable += DebugN1_NetUnavailable;
            command.Error += DebugN1_Error;
        }

        public void LeaveHop()
        {

            var command = BiuIoManager.Instance.Handlers[NetDebugLogTransmissionManager.Handler_DebugN1].Command;
            command.Accepted -= DebugN1_Accepted;
            command.Connected -= DebugN1_Connected;
            command.CloseProactive -= DebugN1_CloseProactive;
            command.ClosePassive -= DebugN1_ClosePassive;
            command.WriteDone -= DebugN1_WriteDone;
            command.Timeout -= DebugN1_Timeout;
            command.File -= DebugN1_File;
            command.FileNext -= DebugN1_FileNext;
            command.ConnectionReset -= DebugN1_ConnectionReset;
            command.ConnectRefused -= DebugN1_ConnectRefused;
            command.HostUnavailable -= DebugN1_HostUnavailable;
            command.NetNameUnavailable -= DebugN1_NetNameUnavailable;
            command.NetUnavailable -= DebugN1_NetUnavailable;
            command.Error -= DebugN1_Error;
        }

        public IOOperation HopIo(string name, ulong action, Peer peer, ref bool handled)
        {
            try
            {
                return _actionTable.Dispatch(name, action, peer, ref handled);
            }
            catch
            {
                throw;
            }
        }

        #endregion // ITransmissionHop


        private void UserLogin(Peer peer)
        {           
            try
            {
                _server.ServerContext.AddPeer(peer.RemoteKey, new UserInfo() { UID = (int)peer.RemoteKey });
            }
            catch
            {
                throw;
            }
        }

        private void UserLogout(Peer peer)
        {
            try
            {
                var portalContext = _server.ServerContext;
                if (!peer.IsServer && portalContext.IsPeerLogin(peer.RemoteKey))
                {
                    var userContext = portalContext.GetUserContext(peer.RemoteKey);
                    portalContext.RemovePeer(peer.RemoteKey);
                }
            }
            catch
            {
            }
        }


        #region DebugN1 Command
        void DebugN1_Accepted(Peer peer, BoolArg arg)
        {
            arg.RetuenValue = true;

            if (null != peer.SecurityContext)
            {
                RawData raw = new RawData()
                {
                    Raw = peer.SecurityContext.ExportKey().ToList()
                };

                AshleyHeader header = Ashley.CreateHeader(AshleyVersion.V1_1, NetDebugLogTransmissionManager.ID_N1, 
                    (ulong)BiuAction.PublicKey, AshleyFlag.None);
                peer.Send(raw, header);
            }

            try
            {
                UserLogin(peer);
            }
            catch
            {
            }
        }

        void DebugN1_Connected(Peer peer)
        {
            // ***** <Generated Code> [Client-Data-Peer] Client handles pending data transmission (initiated from client-main-peer) *****
            /*
            int id = (int)peer.Var;
            var dataInfo = peer.GetDataInfo(id);
            if (null != dataInfo)
            {
                if (dataInfo.Direction)
                {
                    peer.ClientSendFileFinal(id);
                }
                else
                {
                    peer.ClientRecvFileFinal(id);
                }
            }
            */
        }

        void DebugN1_ClosePassive(Peer peer)
        {
            try
            {
                UserLogout(peer);
            }
            catch
            {
            }
        }

        void DebugN1_CloseProactive(Peer peer)
        {
            try
            {
                UserLogout(peer);
            }
            catch
            {
            }
        }

        void DebugN1_WriteDone(Peer peer, int size)
        {

        }

        void DebugN1_Timeout(Peer peer)
        {

        }

        void DebugN1_File(Peer peer, DataInfo dataInfo)
        {

        }

        void DebugN1_FileNext(Peer peer, DataContinuation continuation, WaitDataInfo waitDataInfo)
        {

        }

        void DebugN1_ConnectionReset(Peer peer)
        {

        }

        void DebugN1_ConnectRefused(Peer peer)
        {

        }

        void DebugN1_HostUnavailable(Peer peer)
        {

        }

        void DebugN1_NetNameUnavailable(Peer peer)
        {
            try
            {
                UserLogout(peer);
            }
            catch
            {
            }
        }

        void DebugN1_NetUnavailable(Peer peer)
        {

        }

        void DebugN1_Error(Peer peer, ulong ecs, int error)
        {

        }
        #endregion // DebugN1 Command


        #region DebugN1 IOs

        private IOOperation DebugN1_PublicKey(BiuIoHopArgs arg)
        {
            var peer = arg.Peer;
            try
            {
                RawData raw = new RawData();
                if (!peer.Protocol.GetObjKept<RawData>(ref raw))
                {
                    return IOOperation.None;
                }

                AshleyHeader header = Ashley.CreateHeader(AshleyVersion.V1_1, NetDebugLogTransmissionManager.ID_N1, 
                    (ulong)BiuAction.Ack, AshleyFlag.None);

                Ack ack = new Ack()
                {
                    AckAction = (ulong)BiuAction.PublicKey,
                    AckResult = (uint)BiuAckResult.OK
                };

                if (null == peer.SecurityContext)
                {
                    ack.AckResult = (uint)BiuAckResult.Unsupported;
                }
                else if (!peer.SecurityContext.ChangeRemoateKey(raw.Raw.ToArray()))
                {
                    ack.AckResult = (uint)BiuAckResult.Failed;
                }

                peer.Return(ack, header);

                return IOOperation.Continue;
            }
            catch
            {
                throw;
            }
        }

        #endregion
    }
}
