#include "Hooks.h"
#include "RageBackTracking.h"
#include "Ragebot.h"
#include "Resolver.h"
#include "AntiAims.h"
CMBacktracking* g_Backtrack = new CMBacktracking();


//static std::deque<CIncomingSequence>sequences;
//static int lastincomingsequencenumber;
//int Real_m_nInSequencenumber;
//
//void CMBacktracking::UpdateIncomingSequences()
//{
//	if (!csgo->client_state)
//		return;
//	//DWORD ClientState = (DWORD)*(DWORD*)csgo->client_state;
//	//if (ClientState)
//	{
//		NetChannel *netchan = interfaces.engine->GetNetChannel();
//		if (netchan)
//		{
//			if (netchan->m_nInSequenceNr > lastincomingsequencenumber)
//			{
//				lastincomingsequencenumber = netchan->m_nInSequenceNr;
//				sequences.push_front(CIncomingSequence(netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, interfaces.global_vars->realtime));
//			}
//
//			if (sequences.size() > 2048)
//				sequences.pop_back();
//		}
//	}
//}
//
//void CMBacktracking::ClearIncomingSequences()
//{
//	sequences.clear();
//}
//
//void CMBacktracking::AddLatencyToNetchan(NetChannel *netchan, float Latency)
//{
//	for (auto& seq : sequences)
//	{
//		if (interfaces.global_vars->realtime - seq.curtime >= Latency || interfaces.global_vars->realtime - seq.curtime > 1 || interfaces.global_vars->realtime < seq.curtime)
//		{
//			netchan->m_nInReliableState = seq.inreliablestate;
//			netchan->m_nInSequenceNr = seq.sequencenr;
//			break;
//		}
//	}
//}

void CMBacktracking::DoPingSpike(INetChannel* net_channel)
{
	if (!csgo->local /*|| !csgo->local->isAlive()*/)
		return;

	auto ping_value = 0.2f;
	INetChannelInfo* nci = interfaces.engine->GetNetChannelInfo();
	if (nci)
	{
		//ping_value -= nci->GetLatency(FLOW_INCOMING);
		ping_value -= nci->GetLatency(FLOW_OUTGOING);
	}
	static auto oldseqnum = 0;

	if (oldseqnum == net_channel->iInSequenceNr)
		return;

	oldseqnum = net_channel->iInSequenceNr;

	net_channel->iInSequenceNr += 126 - static_cast<uint32_t>(63 * (ping_value));
}